#include "ai.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdatomic.h>

#define SEARCH_TIME_LIMIT_MS 500

typedef struct
{
    atomic_bool *stop_search;
    int timeout_ms;
} SearchTimerArgs;

static bool search_should_stop(const SearchInfo *info)
{
    return info != NULL && info->stop_search != NULL && atomic_load(info->stop_search);
}

static void *search_timer_worker(void *arg)
{
    SearchTimerArgs *timer_args = (SearchTimerArgs *)arg;
    struct timespec delay = {
        .tv_sec = timer_args->timeout_ms / 1000,
        .tv_nsec = (timer_args->timeout_ms % 1000) * 1000000L,
    };

    nanosleep(&delay, NULL);
    atomic_store(timer_args->stop_search, true);

    return NULL;
}

static bool allocate_thread_resources(int nbmoves, pthread_t **threads, SearchThreadArgs **args)
{
    *threads = malloc(sizeof(**threads) * nbmoves);
    *args = malloc(sizeof(**args) * nbmoves);

    if (*threads == NULL || *args == NULL)
    {
        free(*threads);
        free(*args);
        return false;
    }

    return true;
}

static void init_scored_moves(ScoredMove scored_moves[MAX_LEGAL_MOVES], Move legal_moves[MAX_LEGAL_MOVES], int nbmoves)
{
    for (int i = 0; i < nbmoves; i++)
    {
        scored_moves[i].move = legal_moves[i];
        scored_moves[i].score = 0;
    }
}

static ScoredMove create_terminal_score(int depth)
{
    ScoredMove terminal = {0};
    terminal.score = -MAT - depth;
    return terminal;
}

static ScoredMove evaluate_root_move(Chessboard *board, Move move, int depth, SearchInfo *info)
{
    SearchInfo local_info = {0};
    local_info.stop_search = info->stop_search;

    play_move(board, move);
    ScoredMove result = {0};
    result.score = -alpha_beta(board, depth - 1, -INFINI, INFINI, &local_info);
    result.move = move;
    unplay_move(board, move);

    info->nb_positions_evaluated += local_info.nb_positions_evaluated;
    return result;
}

static void *search_best_move_worker(void *arg)
{
    SearchThreadArgs *args = (SearchThreadArgs *)arg;
    SearchInfo local_info = {0};
    local_info.stop_search = args->stop_search;

    play_move(&args->board, args->move);
    args->result.score = -alpha_beta(&args->board, args->depth - 1, args->alpha, args->beta, &local_info);
    args->result.move = args->move;
    args->nb_positions_evaluated = local_info.nb_positions_evaluated;

    if (args->board.hashtable != NULL)
        free(args->board.hashtable);

    return NULL;
}

static ScoredMove evaluate_moves_sequentially(Chessboard *board, ScoredMove *scored_moves, int nbmoves, int depth, SearchInfo *info)
{
    ScoredMove best = {0};
    best.score = -INFINI;

    for (int i = 0; i < nbmoves; i++)
    {
        if (search_should_stop(info))
            break;

        ScoredMove result = evaluate_root_move(board, scored_moves[i].move, depth, info);
        scored_moves[i].score = result.score;

        if (result.score > best.score)
            best = result;
    }
    return best;
}

static void launch_search_threads(Chessboard *board, ScoredMove *scored_moves, int nbmoves, int depth, pthread_t *threads, SearchThreadArgs *args, int alpha, int beta, atomic_bool *stop_search)
{
    for (int i = 0; i < nbmoves; i++)
    {
        copy_chessboard(board, &args[i].board);

        args[i].move = scored_moves[i].move;
        args[i].depth = depth;
        args[i].result = (ScoredMove){0};
        args[i].nb_positions_evaluated = 0;
        args[i].alpha = alpha;
        args[i].beta = beta;
        args[i].stop_search = stop_search;

        pthread_create(&threads[i], NULL, search_best_move_worker, &args[i]);
    }
}

static ScoredMove collect_thread_results(int nbmoves, pthread_t *threads, SearchThreadArgs *args, ScoredMove *scored_moves, SearchInfo *info)
{
    ScoredMove best = {0};
    best.score = -INFINI;

    for (int i = 0; i < nbmoves; i++)
    {
        pthread_join(threads[i], NULL);
        info->nb_positions_evaluated += args[i].nb_positions_evaluated;
        scored_moves[i].score = args[i].result.score;

        if (args[i].result.score > best.score)
            best = args[i].result;
    }
    return best;
}

static ScoredMove search_best_move_mt(Chessboard *board, int depth, SearchInfo *info, int alpha, int beta)
{
    if (search_should_stop(info))
        return create_terminal_score(depth);

    Move legal_moves[MAX_LEGAL_MOVES];
    int nbmoves = get_all_legal_moves(board, legal_moves);

    if (nbmoves == 0)
        return create_terminal_score(depth);

    ScoredMove scored_moves[MAX_LEGAL_MOVES];
    init_scored_moves(scored_moves, legal_moves, nbmoves);

    if (nbmoves <= 1 || depth <= 1)
        return evaluate_moves_sequentially(board, scored_moves, nbmoves, depth, info);

    pthread_t *threads;
    SearchThreadArgs *args;

    if (!allocate_thread_resources(nbmoves, &threads, &args))
        return evaluate_moves_sequentially(board, scored_moves, nbmoves, depth, info);

    launch_search_threads(board, scored_moves, nbmoves, depth, threads, args, alpha, beta, info->stop_search);

    ScoredMove best = collect_thread_results(nbmoves, threads, args, scored_moves, info);

    free(threads);
    free(args);

    return best;
}

static ScoredMove search_best_move_iterative_deepening(Chessboard *board, SearchInfo *info)
{
    ScoredMove best = {0};
    int total_positions = 0;
    int reached_depth = 0;
    atomic_bool stop_search;
    pthread_t timer_thread;
    SearchTimerArgs timer_args = {
        .stop_search = &stop_search,
        .timeout_ms = SEARCH_TIME_LIMIT_MS,
    };

    atomic_init(&stop_search, false);
    info->stop_search = &stop_search;

    pthread_create(&timer_thread, NULL, search_timer_worker, &timer_args);

    int alpha = -INFINI;
    int beta = INFINI;

    int current_depth = 1;

    while (!search_should_stop(info))
    {
        int delta = 50;
        if (current_depth == 1)
        {
            alpha = -INFINI;
            beta = INFINI;
        }
        else
        {
            // alpha = best.score - delta;
            // beta = best.score + delta;
            alpha = -INFINI;
            beta = INFINI; /// pour l'instant je désactive l'apiration window
        }

        ScoredMove result;
        while (1)
        {
            if (search_should_stop(info))
                break;

            info->nb_positions_evaluated = 0;

            result = search_best_move_mt(board, current_depth, info, alpha, beta);

            if (search_should_stop(info))
                break;

            total_positions += info->nb_positions_evaluated;
            if (result.score <= alpha)
            {
                alpha = -INFINI;
                beta = result.score + delta;
                continue;
            }

            if (result.score >= beta)
            {
                alpha = result.score - delta;
                beta = INFINI;
                continue;
            }
            break;
        }

        if (search_should_stop(info))
            break;

        best = result;
        reached_depth = current_depth;

        current_depth++;
    }

    pthread_cancel(timer_thread);
    pthread_join(timer_thread, NULL);

    info->nb_positions_evaluated = total_positions;
    info->depth = reached_depth;

    return best;
}

SearchInfo get_best_move(Chessboard board)
{
    SearchInfo info = {0};

    info.move = search_best_move_iterative_deepening(&board, &info);
    snprintf(info.log, SEARCH_LOG_SIZE, "depth=%d nb_positions=%d evaluation=%d ", info.depth, info.nb_positions_evaluated, info.move.score);
    return info;
}