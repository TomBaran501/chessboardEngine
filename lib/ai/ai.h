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

#define INFINI 1000000000
#define MAT 900000000
#define MAX_DEPTH 50

#pragma once
typedef struct
{
    Move move;
    int score; // score obtenu à la profondeur précédente
} ScoredMove;


/// @brief Run the search for the best move in the position
/// @param board
/// @return
Move get_best_move(Chessboard board);

int alpha_beta(Chessboard *board, int depth, int alpha, int beta, int is_maximizing);
