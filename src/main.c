#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"
#include "chessboardcontroller.h"

#include "stdlib.h"
#include "string.h"

void run_game()
{
    Chessboard board;
    char move[10];

    init_chessboard(&board);
    init_bitboards();

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

        if (!play_move(&board, m))
        {
            printf("Coup invalide. Réessayez.\n");
            continue;
        }

        print_chessboard(&board);
    }
}

int main()
{
    run_game();
    return 0;
}