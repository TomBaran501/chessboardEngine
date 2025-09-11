#include "chessboard/chessboard.h"
#include "chessboard/chessboardcontroller.h"
#include "tools/dynamic_list.h"

#include <criterion/criterion.h>
#include <time.h>

char *start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int run_test(Chessboard board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    GenericList *listescoups = malloc(sizeof(GenericList));
    list_init(listescoups, sizeof(Move));
    listescoups = getalllegalmoves(&board);

    int total = 0;

    for (int i = 0; i < listescoups->size; i++)
    {
        int nbcoups;
        Move *move = (Move *)list_get(listescoups, i);
        play_move(&board, *move);
        nbcoups = run_test(board, profondeur - 1);
        unplay_move(&board, *move);
        total += nbcoups;
    }
    return total;
}

int perft_test(char *fen, int profondeur)
{
    Chessboard board;
    clock_t debut, fin;
    double temps;
    int nbcoups;

    init_chessboard_from_fen(&board, fen);

    printf("début_test ... \n");
    debut = clock();
    nbcoups = run_test(board, profondeur);
    fin = clock();
    temps = ((double)(fin - debut)) / CLOCKS_PER_SEC;
    printf("nb de coups à la prondeur %i: %i en %f secondes\n", profondeur, nbcoups, temps);
    return nbcoups;
}

Test(perft_result_pos_initiale, perft_test)
{
    cr_assert_eq(perft_test(start_pos, 1), 20, " test à la profondeur 1");
    cr_assert_eq(perft_test(start_pos, 2), 400, " test à la profondeur 2");
    cr_assert_eq(perft_test(start_pos, 3), 8902, " test à la profondeur 3");
    cr_assert_eq(perft_test(start_pos, 4), 197281, " test à la profondeur 4");
    cr_assert_eq(perft_test(start_pos, 5), 4865609, " test à la profondeur 5");
}