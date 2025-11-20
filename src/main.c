#include "chessboard/chessboard.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"
#include "ui/rendering.h"
#include "chessboard/perft.h"

#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define MAX_CMD 512
#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

static void run_game(const char *start_fen)
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

void handle_play_cmd(char buffer[MAX_CMD], char current_fen[256])
{
    if (strncmp(buffer, "go play ui", 10) == 0)
    {
        if (strncmp(buffer, "go play ui -wb", 14) == 0 || strncmp(buffer, "go play ui -bw", 14) == 0)
            ui_main_loop(current_fen, AvA);
        else if (strncmp(buffer, "go play ui -w", 13) == 0)
            ui_main_loop(current_fen, WHITE);
        else if (strncmp(buffer, "go play ui -b", 13) == 0)
            ui_main_loop(current_fen, BLACK);

        else
            ui_main_loop(current_fen, HvH);
    }
    else
        run_game(current_fen);

    printf("End of the game ...\n\n");
}

void run_bot_match(char buffer[MAX_CMD])
{
    char bot1_path[256], bot2_path[256];
    int nb_matches = 0;

    if (sscanf(buffer, "botmatch %255s %255s %d", bot1_path, bot2_path, &nb_matches) != 3)
    {
        printf("Usage: botmatch <bot1_path> <bot2_path> <num_matches>\n");
        return;
    }

    if (nb_matches <= 0)
    {
        printf("Error: number of matches must be > 0.\n");
        return;
    }

    ui_bot_match("", "");
}

int main(void)
{
    char buffer[MAX_CMD];
    char current_fen[256] = STARTPOS; // position courante sauvegardée

    printf("\n\n\n\n\n\n Chessboard Engine ready\n\n\n\n\n");

    while (fgets(buffer, sizeof(buffer), stdin))
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strncmp(buffer, "quit", 4) == 0)
            break;

        else if (strncmp(buffer, "position fen ", 13) == 0)
        {
            strncpy(current_fen, buffer + 13, sizeof(current_fen) - 1);
            printf("Position enregistrée (FEN): %s\n", current_fen);
        }

        else if (strncmp(buffer, "position startpos", 17) == 0)
        {
            strncpy(current_fen, STARTPOS, sizeof(current_fen) - 1);
            printf("Position enregistrée (startpos)\n");
        }

        else if (strncmp(buffer, "go perft ", 9) == 0)
        {
            int depth = atoi(buffer + 9);
            perft_test(current_fen, depth);
        }

        else if (strncmp(buffer, "go play", 7) == 0)
            handle_play_cmd(buffer, current_fen);

        else if (strncmp(buffer, "help", 7) == 0)
            printf("Command list:\n- position fen 'fen code' \n- position startpos \n- go perft n \n- go play\n- go play ui -c (replace 'c' with 'b' for black and 'w' for white depneding on wich color you want the ai to play)\n\n");

        else if (strncmp(buffer, "botmatch", 8) == 0)
            run_bot_match(buffer);

        else
            printf("Commande inconnue: %s\nTry 'help' for information on commands\n", buffer);
    }

    printf("Bye!\n");
    return 0;
}
