#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"

#include <criterion/criterion.h>
#include <time.h>

char *start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char *pos2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -0 1";
char *pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
char *pos4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
char *pos6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

int run_test(Chessboard *board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    Move liste_coups[250];
    int nbmoves = get_all_legal_moves(board, liste_coups);

    int total = 0;

    for (int i = 0; i < nbmoves; i++)
    {

        play_move(board, liste_coups[i]);
        total += run_test(board, profondeur - 1);
        unplay_move(board, liste_coups[i]);
    }
    return total;
}

int tperft_test(char *fen, int profondeur)
{
    Chessboard board;
    int nbcoups;

    init_chessboard_from_fen(&board, fen);

    nbcoups = run_test(&board, profondeur);
    return nbcoups;
}

Test(perft_result_pos_initiale, tperft_test)
{
    printf("PERFTS TEST ... \n");
    cr_assert_eq(tperft_test(start_pos, 1), 20, " test à la profondeur 1");
    cr_assert_eq(tperft_test(start_pos, 2), 400, " test à la profondeur 2");
    cr_assert_eq(tperft_test(start_pos, 3), 8902, " test à la profondeur 3");
    cr_assert_eq(tperft_test(start_pos, 4), 197281, " test à la profondeur 4");
    cr_assert_eq(tperft_test(start_pos, 5), 4865609, " test à la profondeur 5");
    //cr_assert_eq(tperft_test(start_pos, 6), 119060324, " test à la profondeur 6");
}

Test(perft_pos2, tperft_test)
{
    cr_assert_eq(tperft_test(pos2, 1), 48, " test à la profondeur 1");
    cr_assert_eq(tperft_test(pos2, 2), 2039, " test à la profondeur 2");
    cr_assert_eq(tperft_test(pos2, 3), 97862, " test à la profondeur 3");
    cr_assert_eq(tperft_test(pos2, 4), 4085603, " test à la profondeur 4");
    //cr_assert_eq(tperft_test(pos2, 5), 193690690, " test à la profondeur 5");
}

Test(perft_pos3, tperft_test)
{
    cr_assert_eq(tperft_test(pos3, 1), 14, " test à la profondeur 1");
    cr_assert_eq(tperft_test(pos3, 2), 191, " test à la profondeur 2");
    cr_assert_eq(tperft_test(pos3, 3), 2812, " test à la profondeur 3");
    cr_assert_eq(tperft_test(pos3, 4), 43238, " test à la profondeur 4");
    cr_assert_eq(tperft_test(pos3, 5), 674624, " test à la profondeur 5");
    cr_assert_eq(tperft_test(pos3, 6), 11030083, " test à la profondeur 6");
    // cr_assert_eq(tperft_test(pos3, 7), 178633661, " test à la profondeur 7");
}

Test(perft_pos4, tperft_test)
{
    cr_assert_eq(tperft_test(pos4, 1), 6, " test à la profondeur 1");
    cr_assert_eq(tperft_test(pos4, 2), 264, " test à la profondeur 2");
    cr_assert_eq(tperft_test(pos4, 3), 9467, " test à la profondeur 3");
    cr_assert_eq(tperft_test(pos4, 4), 422333, " test à la profondeur 4");
    //cr_assert_eq(tperft_test(pos4, 5), 15833292, " test à la profondeur 5");
}

Test(perft_pos6, tperft_test)
{
    cr_assert_eq(tperft_test(pos6, 1), 46, " test à la profondeur 1");
    cr_assert_eq(tperft_test(pos6, 2), 2079, " test à la profondeur 2");
    cr_assert_eq(tperft_test(pos6, 3), 89890, " test à la profondeur 3");
    cr_assert_eq(tperft_test(pos6, 4), 3894594, " test à la profondeur 4");
    //cr_assert_eq(tperft_test(pos6, 5), 164075551, " test à la profondeur 5");
}
