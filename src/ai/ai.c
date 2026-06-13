#include "ai.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    Chessboard board;
    Move move;
    int depth;
    ScoredMove result;
    int nb_positions_evaluated;
} SearchThreadArgs;

static void *search_best_move_worker(void *arg)
{
    SearchThreadArgs *args = (SearchThreadArgs *)arg;
    SearchInfo local_info = {0};

    play_move(&args->board, args->move);

    args->result = search_best_move(&args->board, args->depth - 1, &local_info);
    args->result.score = -args->result.score;
    args->result.move = args->move;
    args->nb_positions_evaluated = local_info.nb_positions_evaluated;

    if (args->board.hashtable != NULL)
        free(args->board.hashtable);

    return NULL;
}

static ScoredMove search_best_move_mt(Chessboard *board, int depth, SearchInfo *info)
{
    Move legal_moves[MAX_LEGAL_MOVES];
    int nbmoves = get_all_legal_moves(board, legal_moves);

    if (nbmoves <= 1 || depth <= 1)
        return search_best_move(board, depth, info);

    pthread_t *threads = malloc(sizeof(*threads) * nbmoves);
    SearchThreadArgs *args = malloc(sizeof(*args) * nbmoves);

    if (threads == NULL || args == NULL)
    {
        free(threads);
        free(args);
        return search_best_move(board, depth, info);
    }

    ScoredMove best = {0};
    best.score = -INFINI;
    int total_positions = 0;

    for (int i = 0; i < nbmoves; i++)
    {
        copy_chessboard(board, &args[i].board);
        args[i].move = legal_moves[i];
        args[i].depth = depth;
        args[i].result = (ScoredMove){0};
        args[i].nb_positions_evaluated = 0;

        pthread_create(&threads[i], NULL, search_best_move_worker, &args[i]);
    }

    for (int i = 0; i < nbmoves; i++)
    {
        pthread_join(threads[i], NULL);
        total_positions += args[i].nb_positions_evaluated;

        if (args[i].result.score > best.score)
            best = args[i].result;
    }

    free(threads);
    free(args);

    info->nb_positions_evaluated = total_positions;
    info->depth = depth;

    return best;
}

SearchInfo get_best_move(Chessboard board)
{
    SearchInfo info = {0};
    info.depth = 4;

    info.move = search_best_move_mt(&board, info.depth, &info);
    snprintf(info.log, SEARCH_LOG_SIZE, "depth=%d nb_positions=%d evaluation=%d ", info.depth, info.nb_positions_evaluated, info.move.score);
    return info;
}