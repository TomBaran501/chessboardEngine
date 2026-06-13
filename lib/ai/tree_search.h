#include "chessboard/chessboard.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"
#include "evaluation.h"

#define MAX_LEGAL_MOVES 250

#define MAT 900000
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

} SearchInfo;


/// @brief Renvoie le meileur coup et son évaluation via iterative deepening
/// @param board 
/// @param depth 
/// @param info 
/// @return
ScoredMove search_best_move(Chessboard *board, int depth, SearchInfo *info);
