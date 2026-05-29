#include <stdio.h>
#include <criterion/criterion.h>
#include <string.h>

#include "ai/ai.h"
#include "chessboard/chessboard.h"
#include "chessboard/move/move.h"

TestSuite(ai_search);

Test(ai_search, black_turn_selects_best_tactical_move)
{
    Chessboard board;

    initialize_tables();
    init_chessboard_from_fen(&board, "4k3/8/8/8/3q4/8/4r3/4K3 b - - 0 1");

    SearchInfo info = get_best_move(board);
    char move[10] = {0};
    move_to_string(&info.move.move, move);

    fprintf(stderr, "chosen move=%s score=%d\n", move, info.move.score);
    fprintf(stderr, "len=%zu raw=%c%c%c%c next=%d next2=%d\n", strlen(move), move[0], move[1], move[2], move[3], (unsigned char)move[4], (unsigned char)move[5]);
    cr_assert_str_eq(move, "e2f2", "Black should choose the best evaluated tactical move after perspective correction");
    cr_assert(info.move.score >= 0, "The chosen move should not be penalized for the side to move");
}
