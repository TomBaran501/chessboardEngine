#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"
#include "evaluation.h"

#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define MAT 900000
#define INFINI 100000000
#define MAX_DEPTH 50

#pragma once
typedef struct
{
    Move move;
    int score; // score obtenu à la profondeur précédente
} ScoredMove;

typedef struct
{
    int depth;
    int nb_positions_evaluated;
} SearchInfo;



/// @brief Run the search for the best move in the position
/// @param board
/// @return
Move get_best_move(Chessboard board);

