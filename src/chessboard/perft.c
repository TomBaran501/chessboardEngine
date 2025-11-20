#include "perft.h"


static int run_test(Chessboard *board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    Move liste_coups[250];
    int nbmoves = get_all_legal_moves(board, liste_coups);

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

static void *thread_worker(void *arg)
{
    ThreadArgs *targs = (ThreadArgs *)arg;

    play_move(&targs->board, targs->move);
    targs->result = run_test(&targs->board, targs->profondeur - 1);
    return NULL;
}

static uint64_t run_test_mt(Chessboard *board, int profondeur)
{
    if (profondeur <= 0)
        return 1;

    Move liste_coups[250];
    int nbmoves = get_all_legal_moves(board, liste_coups);

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