#define _POSIX_C_SOURCE 199309L

#include "chessboard/chessboard.h"
#include "chessboard/bitboards/bitboard.h"
#include "chessboard/bitboards/masks.h"
#include "chessboard/move/move.h"
#include "chessboard/chessboardcontroller.h"
#include "ai/ai.h"
#include "ui/rendering.h"

#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define MAX_CMD 512
#define STARTPOS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

typedef struct
{
    Chessboard board;
    Move move;
    int profondeur;
    long long result;
} ThreadArgs;

int run_test(Chessboard *board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    Move liste_coups[250];
    int nbmoves = getalllegalmoves(board, liste_coups);

    int total = 0;

    for (int i = 0; i < nbmoves; i++)
    {
        int nbcoups;

        play_move(board, liste_coups[i]);
        nbcoups = run_test(board, profondeur - 1);
        unplay_move(board, liste_coups[i]);
        total += nbcoups;
    }
    return total;
}

void *thread_worker(void *arg)
{
    ThreadArgs *targs = (ThreadArgs *)arg;

    play_move(&targs->board, targs->move);
    targs->result = run_test(&targs->board, targs->profondeur - 1);
    return NULL;
}

uint64_t run_test_mt(Chessboard *board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    Move liste_coups[250];
    int nbmoves = getalllegalmoves(board, liste_coups);

    pthread_t *threads = malloc(sizeof(pthread_t) * nbmoves);
    ThreadArgs *args = malloc(sizeof(ThreadArgs) * nbmoves);

    long long total = 0;

    // Lancer un thread par coup
    for (int i = 0; i < nbmoves; i++)
    {
        args[i].board = *board;
        args[i].move = liste_coups[i];
        args[i].profondeur = profondeur;
        args[i].result = 0;

        pthread_create(&threads[i], NULL, thread_worker, &args[i]);
    }

    // Attendre la fin des threads
    for (int i = 0; i < nbmoves; i++)
    {
        pthread_join(threads[i], NULL);
        print_move(&liste_coups[i]);
        printf(": %lld\n", args[i].result);

        total += args[i].result;
    }

    free(threads);
    free(args);

    return total;
}

void perft_test(char *fen, int profondeur)
{
    Chessboard board;
    init_chessboard_from_fen(&board, (char *)fen);

    struct timespec t_start, t_end;
    if (clock_gettime(CLOCK_MONOTONIC, &t_start) != 0)
    {
        perror("clock_gettime start");
    }

    unsigned long long nbcoups = run_test_mt(&board, profondeur);

    if (clock_gettime(CLOCK_MONOTONIC, &t_end) != 0)
    {
        perror("clock_gettime end");
    }

    double elapsed = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    if (elapsed <= 0.0)
        elapsed = 1e-9; // sécurité pour éviter div/0

    printf("nb de coups à la profondeur %d: %llu en %.6f secondes... %.0f coups/s\n",
           profondeur,
           (unsigned long long)nbcoups,
           elapsed,
           (double)nbcoups / elapsed);
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

int main(void)
{
    char buffer[MAX_CMD];
    char current_fen[256] = STARTPOS; // position courante sauvegardée

    printf("\n\n\n\n\n\n Chessboard Engine ready\n\n\n\n\n");

    while (fgets(buffer, sizeof(buffer), stdin))
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strncmp(buffer, "quit", 4) == 0)
        {
            break;
        }
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
            if (strlen(current_fen) == 0)
            {
                printf("Erreur: aucune position enregistrée.\n");
                continue;
            }
            int depth = atoi(buffer + 9);
            perft_test(current_fen, depth);
        }

        else if (strncmp(buffer, "go play", 7) == 0)
        {
            if (strncmp(buffer, "go play ui", 10) == 0)
                if (strncmp(buffer, "go play ui -w", 10) == 0)
                    ui_game_loop(current_fen, WHITE);
                else
                    ui_game_loop(current_fen, BLACK);

            else
                run_game(current_fen);

            printf("End of the game ...\n\n");
        }

        else if (strncmp(buffer, "help", 7) == 0)
            printf("Command list:\n- position fen 'fen code' \n- position startpos \n- go perft n \n- go play\n- go play ui -c (replace 'c' with 'b' for black and 'w' for white depneding on wich color you want the ai to play)\n\n");

        else
            printf("Commande inconnue: %s\nTry 'help' for information on commands\n", buffer);
    }

    printf("Bye!\n");
    return 0;
}