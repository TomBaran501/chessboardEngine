#include "chessboard/chessboard.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"
#include "evaluation.h"
#include "transposition_table.h"

#include <stdatomic.h>

#define MAX_LEGAL_MOVES 250

#define INFINI 100000000
#define MAX_DEPTH 50
#define SEARCH_LOG_SIZE 256

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
    char log[SEARCH_LOG_SIZE];
    ScoredMove move;
    atomic_bool *stop_search;
    int nb_researches;

} SearchInfo;

extern TranspositionTable global_tt;

/// @brief Renvoie le meileur coup et son évaluation à une profondeur donnée
/// @param board
/// @param depth
/// @param info
/// @return
int alpha_beta(Chessboard *board, int depth, int root_depth, int alpha, int beta, SearchInfo *info);
