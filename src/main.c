#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"

#include "stdlib.h"
#include "string.h"
#include <time.h>

char *start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

int run_test(Chessboard *board, int profondeur, int profondeur_max)
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
        nbcoups = run_test(board, profondeur - 1, profondeur_max);
        unplay_move(board, *move);
        // if (profondeur == profondeur_max)
        // {
        //     print_move(move);
        //     printf(": %i \n", nbcoups);
        //     // print_chessboard(&board);
        // }
        total += nbcoups;
    }
    list_free(listescoups);
    free(listescoups);
    return total;
}

int run_test_with_details(Chessboard board, int profondeur)
{
    return run_test(&board, profondeur, profondeur);
}

int perft_test(char *fen, int profondeur)
{
    Chessboard board;
    clock_t debut, fin;
    double temps;
    int nbcoups;

    init_chessboard_from_fen(&board, fen);

    printf("début perft test ... \n");
    debut = clock();
    nbcoups = run_test_with_details(board, profondeur);
    fin = clock();
    temps = ((double)(fin - debut)) / CLOCKS_PER_SEC;
    printf("fin perft test \n");
    printf("nb de coups à la prondeur %i: %i en %f secondes... %i coups/s \n", profondeur, nbcoups, temps, (int)(nbcoups / temps));
    return nbcoups;
}

void run_game(const char *start_fen)
{
    Chessboard board;
    char move[10];
    char *fen = malloc(100);

    init_chessboard_from_fen(&board, start_fen);

    printf("Bienvenue dans le jeu d'échecs en C !\n");
    print_chessboard(&board);

    while (1)
    {
        printf("\n%s to move > ", board.white_to_play ? "White" : "Black");

        if (!fgets(move, sizeof(move), stdin))
        {
            fprintf(stderr, "Erreur de lecture.\n");
            break;
        }

        // Supprimer le saut de ligne
        move[strcspn(move, "\n")] = '\0';

        if (strcmp(move, "exit") == 0 || strcmp(move, "quit") == 0)
        {
            printf("Fin de la partie.\n");
            break;
        }

        Move m = get_move(move);
        if (!try_play_move(&board, &m))
        {
            printf("Coup invalide. Réessayez.\n");
            continue;
        }
        printf("%s\n", return_fen_code(&board, fen));
        print_chessboard(&board);
    }

    free(fen);
}

int main()
{
    perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6);
    run_game("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    return 0;
}