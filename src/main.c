#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"

#include "stdlib.h"
#include "string.h"
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
        if (profondeur == 3)
            printf("coup: %i vers %i: %i \n", move->from, move->to, nbcoups);
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

void run_game()
{
    Chessboard board;
    char move[10];
    char* fen = malloc(100);

    //perft_test("RNBQKBNR/PPP1PPPP/3P4/8/8/8/pppppppp/rnbqkbnr b KQkq - 0 1", 3);

    init_chessboard_from_fen(&board, start_pos);

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
        if (!try_play_move(&board, m))
        {
            printf("Coup invalide. Réessayez.\n");
            continue;
        }
        printf("%s\n",return_fen_code(&board, fen));
        print_chessboard(&board);
    }

    free(fen);
}

int main()
{
    run_game();
    return 0;
}