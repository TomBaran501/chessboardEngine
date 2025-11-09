#include "api_connector.h"

static int read_line_timeout(FILE *stream, char *buf, int size, int timeout_sec)
{
    int fd = fileno(stream);
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
    if (ret == -1)
        return -1; // erreur
    if (ret == 0)
        return -2; // timeout

    if (fgets(buf, size, stream) == NULL)
        return -3; // EOF ou erreur

    buf[strcspn(buf, "\r\n")] = '\0';
    return 0;
}

// --- API ---

int bot_connect(BotConnector *bot, const char *bot_path)
{
    int to_bot[2];   // pipe père -> bot
    int from_bot[2]; // pipe bot -> père

    if (pipe(to_bot) == -1 || pipe(from_bot) == -1)
        return -1;

    pid_t pid = fork();
    if (pid == -1)
        return -1;

    if (pid == 0) // enfant
    {
        // redirection stdin/out
        dup2(to_bot[0], STDIN_FILENO);
        dup2(from_bot[1], STDOUT_FILENO);

        close(to_bot[1]);
        close(from_bot[0]);

        execl(bot_path, bot_path, NULL);
        perror("execl");
        exit(1);
    }

    // père
    close(to_bot[0]);
    close(from_bot[1]);

    bot->bot_in = fdopen(to_bot[1], "w");
    bot->bot_out = fdopen(from_bot[0], "r");
    bot->pid = pid;

    if (!bot->bot_in || !bot->bot_out)
        return -1;

    // Pas de buffering pour éviter les blocages
    setvbuf(bot->bot_in, NULL, _IONBF, 0);
    setvbuf(bot->bot_out, NULL, _IONBF, 0);

    return 0;
}

int is_bot_connected(BotConnector *bot)
{
    return bot && bot->bot_in && bot->bot_out;
}

int bot_set_fen(BotConnector *bot, const char *fen)
{
    if (!is_bot_connected(bot))
        return -1;

    fprintf(bot->bot_in, "fen %s\n", fen);
    fflush(bot->bot_in);
    return 0;
}

int bot_get_best_move(BotConnector *bot, char *move_str)
{
    if (!is_bot_connected(bot))
        return -1;

    fprintf(bot->bot_in, "get move\n");
    fflush(bot->bot_in);

    int res = read_line_timeout(bot->bot_out, move_str, MOVE_SIZE, 3); // timeout 3s

    if (res != 0)
        return -1;

    return 0;
}

int bot_play_move(BotConnector *bot, const char *move_str)
{
    if (!is_bot_connected(bot))
        return -1;

    fprintf(bot->bot_in, "move %s\n", move_str);
    fflush(bot->bot_in);
    return 0;
}

void bot_disconnect(BotConnector *bot)
{
    if (!bot)
        return;

    if (bot->bot_in)
        fclose(bot->bot_in);
    if (bot->bot_out)
        fclose(bot->bot_out);

    if (bot->pid > 0)
        waitpid(bot->pid, NULL, 0);

    bot->bot_in = NULL;
    bot->bot_out = NULL;
    bot->pid = 0;
}
