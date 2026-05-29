#include <criterion/criterion.h>

#include "ai/evaluation.h"
#include "chessboard/chessboard.h"

void setup_evaluation(void)
{
    cr_assert_eq(initialize_tables(), 0, "Tables should initialize successfully");
}

TestSuite(evaluation, .init = setup_evaluation);

Test(evaluation, side_to_move_flips_score)
{
    Chessboard board;

    init_chessboard_from_fen(&board, "8/8/8/8/8/8/1N6/8 w - - 0 1");
    int score_white = evaluate_position(&board);

    board.white_to_play = false;
    int score_black = evaluate_position(&board);

    cr_assert_eq(score_white + score_black, 0, "Changing side to move should invert the evaluation");
}
