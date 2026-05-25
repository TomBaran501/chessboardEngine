#include "api_connector.h"
#include <sys/time.h>

static int read_line_timeout(FILE *stream, char *buf, int size, int timeout_sec)
{
    int fd = fileno(stream);
    int total = 0;
    struct timeval start, now, tv;

    if (size <= 1)
        return -3;

    if (gettimeofday(&start, NULL) == -1)
        return -1;

    while (total < size - 1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        if (gettimeofday(&now, NULL) == -1)
            return -1;

        long elapsed = now.tv_sec - start.tv_sec;
        if (elapsed >= timeout_sec)
            return -2;

        tv.tv_sec = timeout_sec - elapsed;
        tv.tv_usec = 0;

        int ret = select(fd + 1, &readfds, NULL, NULL, &tv);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (ret == 0)
            return -2;

        char c;
        ssize_t n = read(fd, &c, 1);
        if (n == 0)
            return -3;
        if (n == -1)
        {
            if (errno == EINTR)
                continue;
            return -3;
        }

        buf[total++] = c;
        if (c == '\n')
            break;
    }

    buf[total] = '\0';
    while (total > 0 && (buf[total - 1] == '\n' || buf[total - 1] == '\r'))
        buf[--total] = '\0';

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

int bot_get_best_move(BotConnector *bot, char *move_str, char *log_msg, int timeout_sec)
{
    if (!is_bot_connected(bot))
        return -1;

    fprintf(bot->bot_in, "get move\n");
    fflush(bot->bot_in);

    int res = read_line_timeout(bot->bot_out, move_str, MOVE_SIZE, timeout_sec);
    if (res != 0)
        return -1;

    // Essayer de lire un message de log optionnel provenant du bot.
    // On laisse un petit timeout pour le log afin de ne pas bloquer.
    if (log_msg)
    {
            int res2 = read_line_timeout(bot->bot_out, log_msg, LOG_SIZE, 1);
            if (res2 != 0)
                log_msg[0] = '\0';
            else
            {
                // Attendu: ligne de log préfixée par "LOG:". Si présent, on strip le préfixe.
                if (strncmp(log_msg, "LOG:", 4) == 0)
                {
                    char *p = log_msg + 4;
                    while (*p == ' ')
                        p++;
                    memmove(log_msg, p, strlen(p) + 1);
                }
            }
    }

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
