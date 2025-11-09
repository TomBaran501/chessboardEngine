#include <stdio.h>
#include <string.h>
#include "chessboard/chessboardcontroller.h"
#include "chessboard/chessboard.h"
#include "ai/ai.h"

#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define CMD_SIZE 256
#define FEN_SIZE 256

int is_bot_killed(char *command, size_t size)
{
    if (fgets(command, size, stdin) == NULL)
        return 1; 

    command[strcspn(command, "\r\n")] = '\0';

    if (strncmp(command, "quit", 4) == 0)
        return 1;

    return 0;
}

void handle_commands(const char *command, Chessboard *board, char *fen)
{
    if (strncmp(command, "get move", 8) == 0)
    {
        Move best_move = get_best_move(*board);
        print_move(&best_move);
        printf("\n");
        fflush(stdout);
    }
    else if (strncmp(command, "fen ", 4) == 0)
    {
        strncpy(fen, command + 4, FEN_SIZE - 1);
        fen[FEN_SIZE - 1] = '\0';
        init_chessboard_from_fen(board, fen);
    }
    else if (strncmp(command, "move ", 5) == 0)
    {
        char move[10] = {0};
        strncpy(move, command + 5, 4);
        Move m = get_move(move);
        try_play_move(board, &m);
    }
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);

    char fen[FEN_SIZE] = STARTPOS;
    Chessboard board;
    init_chessboard_from_fen(&board, fen);

    char command[CMD_SIZE];

    while (1)
    {
        if (is_bot_killed(command, sizeof(command)))
            break;

        handle_commands(command, &board, fen);
    }

    return 0;
}