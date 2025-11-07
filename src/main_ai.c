#include <stdio.h>
#include <string.h>
#include "chessboard/chessboardcontroller.h"
#include "chessboard/chessboard.h"
#include "ai/ai.h"

int main()
{
    char fen[256];
    Chessboard board;
    char color[256];
    int color_ai;
    int running = 1;

    fgets(fen, sizeof(fen), stdin);
    fgets(color, sizeof(color), stdin);

    color_ai = (strncmp(color, "w", 1) == 0) ? WHITE : BLACK;
    init_chessboard_from_fen(&board, fen);
    initialise_ai(color_ai);

    char command[20];

    while (running)
    {
        if (fgets(command, sizeof(command), stdin) == NULL)
            break;

        if (strncmp(command, "quit", 4) == 0)
            running = 0;

        else if (strncmp(command, "get move", 8) == 0)
        {
            Move best_move = get_best_move(board);
            play_move(&board, best_move);
            print_move(&best_move);
        }
        else if (strncmp(command, "move ", 5) == 0)
        {
            char move[10] = {command[5], command[6], command[7], command[8], command[9], '\0'};
            Move m = get_move(move);
            try_play_move(&board, &m);
        }
    }
    return 0;
}