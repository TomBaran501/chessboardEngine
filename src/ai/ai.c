#include "ai.h"

SearchInfo get_best_move(Chessboard board)
{
    SearchInfo info = {0};
    info.depth = 4;

    info.move = search_best_move(&board, info.depth, &info);
    snprintf(info.log, SEARCH_LOG_SIZE, "depth=%d nb_positions=%d evaluation=%d ", info.depth, info.nb_positions_evaluated, info.move.score);
    return info;
}