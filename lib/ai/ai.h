#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"
#include "evaluation.h"
#include "transposition_tables.h"

#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define INFINI 1000000000
#define MAX_QDEPTH 8
#define MAX_DEPTH 50

typedef struct
{
    Move move;
    int score; // score obtenu à la profondeur précédente
} ScoredMove;

typedef struct
{
    Chessboard board;
    Move move;
    int score;
    unsigned long long nbmoves;
    int true_depth;
} ThreadTask;

/// @brief Run the search for the best move in the position
/// @param board
/// @return
Move get_best_move(Chessboard board);

void initialise_ai();