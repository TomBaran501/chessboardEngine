#include <stdio.h>
#include <string.h>
#include "chessboard/chessboardcontroller.h"
#include "chessboard/chessboard.h"

int main()
{
    char fen[256];
    Chessboard board;

    fgets(fen, sizeof(fen), stdin);
    init_chessboard_from_fen(&board, fen);

    char move[10];

    while (fgets(move, sizeof(move), stdin))
    {
        if (strncmp(move, "move ", 5) == 0)
        {
            Move m = get_move(move);
            try_play_move(&board, &m);
        }
        
    }

    return 0;
}
