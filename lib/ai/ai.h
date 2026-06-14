#include "chessboard/chessboard.h"
#include "chessboard/move/move.h"
#include "tree_search.h"

#include <stdatomic.h>

typedef struct
{
    Chessboard board;
    Move move;
    int depth;
    ScoredMove result;
    int nb_positions_evaluated;
    int alpha;
    int beta;
    atomic_bool *stop_search;
} SearchThreadArgs;

/// @brief Run the search for the best move in the position
/// @param board
/// @return
SearchInfo get_best_move(Chessboard board);
