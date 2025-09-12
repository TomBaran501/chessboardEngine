#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"
#include "tools/dynamic_list.h"

#include <criterion/criterion.h>
#include <time.h>

char *start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char *pos2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -0 1";
char *pos3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";

int run_test(Chessboard *board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    GenericList *listescoups = malloc(sizeof(GenericList));
    list_init(listescoups, sizeof(Move));
    getalllegalmoves(board, listescoups);

    int total = 0;

    for (int i = 0; i < listescoups->size; i++)
    {
        int nbcoups;
        Move *move = (Move *)list_get(listescoups, i);
        play_move(board, *move);
        nbcoups = run_test(board, profondeur - 1);
        unplay_move(board, *move);
        total += nbcoups;
    }
    list_free(listescoups);
    free(listescoups);
    return total;
}

int perft_test(char *fen, int profondeur)
{
    Chessboard board;
    int nbcoups;

    init_chessboard_from_fen(&board, fen);
    save_globals_state();

    nbcoups = run_test(&board, profondeur);
    check_globals_state();
    return nbcoups;
}

Test(perft_result_pos_initiale, perft_test)
{
    // cr_assert_eq(perft_test(start_pos, 1), 20, " test à la profondeur 1");
    // cr_assert_eq(perft_test(start_pos, 2), 400, " test à la profondeur 2");
    // cr_assert_eq(perft_test(start_pos, 3), 8902, " test à la profondeur 3");
    // cr_assert_eq(perft_test(start_pos, 4), 197281, " test à la profondeur 4");
    // cr_assert_eq(perft_test(start_pos, 5), 4865609, " test à la profondeur 5");
    // cr_assert_eq(perft_test(start_pos, 6), 119060324, " test à la profondeur 6");
}

Test(perft_pos2, perft_test)
{
    // cr_assert_eq(perft_test(pos2, 1), 48, " test à la profondeur 1");
    // cr_assert_eq(perft_test(pos2, 2), 2039, " test à la profondeur 2");
    // cr_assert_eq(perft_test(pos2, 3), 97862, " test à la profondeur 3");
    // cr_assert_eq(perft_test(pos2, 4), 4085603, " test à la profondeur 4");
    // cr_assert_eq(perft_test(pos2, 5), 193690690, " test à la profondeur 5");
}

//Check promotions
Test(perft_pos3, perft_test)
{
    // cr_assert_eq(perft_test(pos3, 1), 14, " test à la profondeur 1");
    // cr_assert_eq(perft_test(pos3, 2), 191, " test à la profondeur 2");
    // cr_assert_eq(perft_test(pos3, 3), 2812, " test à la profondeur 3");
    // cr_assert_eq(perft_test(pos3, 4), 43238, " test à la profondeur 4");
    // cr_assert_eq(perft_test(pos3, 5), 674624, " test à la profondeur 5");
    // cr_assert_eq(perft_test(pos3, 6), 11030083, " test à la profondeur 6");
    cr_assert_eq(perft_test(pos3, 7), 178633661, " test à la profondeur 7");
}
