#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"

#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>


typedef struct
{
    Chessboard board;
    Move move;
    int profondeur;
    long long result;
} ThreadArgs;


/// @brief Lance un "perft test" depuis la position fen
/// @param fen 
/// @param profondeur 
void perft_test(char *fen, int profondeur);