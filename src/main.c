#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"

#include "stdlib.h"
#include "string.h"

void run_game()
{
    Chessboard board;
    char move[10];

    //init_chessboard(&board);
    init_chessboard_from_fen(&board, "RNBQKBNR/PPPP1PPP/8/4P3/8/8/pppppppp/rnbqkbnr b - e3 0 1");
    init_bitboards();

    printf("Bienvenue dans le jeu d'échecs en C !\n");
    print_chessboard(&board);

    while (1)
    {
        printf(return_fen_code(&board));
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