#include "ai.h"

static const int piece_value[7] = {
    0,
    900,
    320,
    330,
    100,
    500,
    INFINI,
};

int compare_moves_desc(const void *a, const void *b)
{
    return ((ScoredMove *)b)->score - ((ScoredMove *)a)->score;
}

int get_value_piece(int square, Chessboard *board)
{
    uint64_t piece_pos = create_1bit_board(square);
    if (board->pawns & piece_pos)
        return piece_value[PAWN];
    if (board->queens & piece_pos)
        return piece_value[QUEEN];
    if (board->knights & piece_pos)
        return piece_value[KNIGHT];
    if (board->bishops & piece_pos)
        return piece_value[BISHOP];
    if (board->rooks & piece_pos)
        return piece_value[ROOK];
    if (board->kings & piece_pos)
        return piece_value[KING];
    return 0;
}

int mvv_lva_compare(const void *a, const void *b)
{
    const Move *ma = (const Move *)a;
    const Move *mb = (const Move *)b;

    int victimA = piece_value[ma->piece_taken];
    int victimB = piece_value[mb->piece_taken];

    // if (victimA != victimB)
    return victimB - victimA;

    // int attackerA = piece_value[ma->piece_moved];
    // int attackerB = piece_value[mb->piece_moved];

    // return attackerA - attackerB;
}

int captures_value(Move *a, Chessboard *board)
{
    int victim = piece_value[a->piece_taken];
    int attacker = get_value_piece(a->to, board);
    return victim - attacker;
}

double elapsed_ms(struct timespec start)
{
    struct timespec now;
    clock_gettime(1, &now);
    return (now.tv_sec - start.tv_sec) * 1000.0 +
           (now.tv_nsec - start.tv_nsec) / 1e6;
}

void sort_moves_heuristic(Move *moves, int nbmoves)
{
    ScoredMove temp[250];

    for (int i = 0; i < nbmoves; i++)
    {
        temp[i].move = moves[i];
        temp[i].score = 0;

        if (moves[i].piece_taken)
            temp[i].score += 200;

        if (moves[i].promotion_flag)
            temp[i].score += 400;
    }

    qsort(temp, nbmoves, sizeof(ScoredMove), compare_moves_desc);

    // Remettre dans le tableau original
    for (int i = 0; i < nbmoves; i++)
    {
        moves[i] = temp[i].move;
    }
}

int quiescence_search(Chessboard *board, int alpha, int beta, unsigned long long *nbcoups, int qdepth)
{
    if (qdepth > MAX_QDEPTH)
        return evaluate_position(board);

    int stand_pat = evaluate_position(board);

    if (stand_pat >= beta)
        return beta;

    if (stand_pat > alpha)
        alpha = stand_pat;

    // Générer seulement les captures (et éventuellement checks)
    Move moves[250];
    int nbmoves = get_all_captures(board, moves);
    *nbcoups += nbmoves;

    qsort(moves, nbmoves, sizeof(Move), mvv_lva_compare);

    for (int i = 0; i < nbmoves; i++)
    {
        // delta pruning (optionnel mais accélère beaucoup)
        int captured_value = piece_value[moves[i].piece_taken];
        if (stand_pat + captured_value + 200 < alpha)
            continue;

        play_move(board, moves[i]);
        int score = -quiescence_search(board, -beta, -alpha, nbcoups, qdepth + 1);
        unplay_move(board, moves[i]);

        if (score >= beta)
            return score;

        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

int alphabeta(Chessboard *board, int depth, int alpha, int beta, unsigned long long *nbcoups)
{
    if (depth == 0)
        return quiescence_search(board, alpha, beta, nbcoups, 0);

    Move moves[250];
    int nbmoves = getalllegalmoves(board, moves);
    *nbcoups += nbmoves;

    if (nbmoves == 0)
    {
        if (is_check(board))
            return -INFINI - depth; // mat rapide = meilleur

        else
            return 0;
    }

    sort_moves_heuristic(moves, nbmoves);
    int value = -INFINI;

    for (int i = 0; i < nbmoves; i++)
    {
        play_move(board, moves[i]);
        int score = -alphabeta(board, depth - 1, -beta, -alpha, nbcoups);
        unplay_move(board, moves[i]);

        if (score > value)
            value = score;

        if (value > alpha)
            alpha = value;

        if (alpha >= beta)
            return alpha;
    }
    return value;
}

int iterative_deepening(Chessboard *board, unsigned long long *nbcoups, int time_limit_ms, int *true_depht)
{
    Move moves[250];
    ScoredMove scored_moves[250];
    int nbmoves = getalllegalmoves(board, moves);

    if (nbmoves == 0)
    {
        if (is_check(board))
            return -INFINI - MAX_DEPTH;
        else
            return 0;
    }

    for (int i = 0; i < nbmoves; i++)
    {
        scored_moves[i].move = moves[i];
        scored_moves[i].score = 0;
    }

    struct timespec start;
    clock_gettime(1, &start);

    int best_score = -INFINI;

    for (int d = 1; d <= MAX_DEPTH; d++)
    {
        int window = 50;
        int alpha = (best_score == -INFINI) ? -INFINI : best_score - window;
        int beta = (best_score == -INFINI) ? INFINI : best_score + window;
        int score;

        while (1)
        {
            int local_best = -INFINI;

            for (int i = 0; i < nbmoves; i++)
            {
                if (elapsed_ms(start) > time_limit_ms)
                {
                    *true_depht = d - 1;
                    return best_score;
                }

                play_move(board, scored_moves[i].move);
                int val = -alphabeta(board, d - 1, -beta, -alpha, nbcoups);
                unplay_move(board, scored_moves[i].move);

                scored_moves[i].score = val;

                if (val > local_best)
                    local_best = val;
            }
            score = local_best;

            if (score <= alpha) // fail-low
            {
                alpha = score - window;
                beta = score + window;
                continue;
            }
            else if (score >= beta) // fail-high
            {
                alpha = score - window;
                beta = score + window;
                continue;
            }
            else
                break;
        }
        qsort(scored_moves, nbmoves, sizeof(ScoredMove), compare_moves_desc);
        best_score = scored_moves[0].score;
    }

    return best_score;
}

// --- Gestion du multithreading ---
void *thread_worker_ai(void *arg)
{
    ThreadTask *task = (ThreadTask *)arg;

    Chessboard local_board = task->board;
    play_move(&local_board, task->move);
    unsigned long long nbcoups = 0;
    int true_depth = 0;
    task->score = -iterative_deepening(&local_board, &nbcoups, 300, &true_depth);
    task->nbmoves = nbcoups;
    task->true_depth = true_depth + 1;

    return NULL;
}

Move get_best_move(Chessboard board)
{
    Move moves[250];
    int nbmoves = getalllegalmoves(&board, moves);
    int total_moves = 0;
    double depth = 0;

    pthread_t threads[250];
    ThreadTask tasks[250];

    struct timespec t_start, t_end;
    if (clock_gettime(1, &t_start) != 0)
        perror("clock_gettime start");

    // Lancer un thread par coup légal
    for (int i = 0; i < nbmoves; i++)
    {
        tasks[i].board = board; // copie de la position
        tasks[i].move = moves[i];
        tasks[i].score = -INFINI;
        tasks[i].nbmoves = 0;
        tasks[i].true_depth = 0;

        pthread_create(&threads[i], NULL, thread_worker_ai, &tasks[i]);
    }

    // Attendre les threads
    for (int i = 0; i < nbmoves; i++)
    {
        pthread_join(threads[i], NULL);
        total_moves += tasks[i].nbmoves;
        depth += tasks[i].true_depth;
    }

    depth /= nbmoves;

    // Choisir le meilleur coup
    int best_score = -INFINI;
    int best_index = 0;
    for (int i = 0; i < nbmoves; i++)
    {
        if (tasks[i].score >= best_score)
        {
            best_score = tasks[i].score;
            best_index = i;
        }
    }

    if (clock_gettime(1, &t_end) != 0)
        perror("clock_gettime end");

    double elapsed = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    if (elapsed <= 0.0)
        elapsed = 1e-9;

    print_move(&moves[best_index]);
    printf(" is the best move with score: %d at average depth %f searching %i moves... en %.6f secondes... %.0f coups/s\n\n",
           best_score, depth, total_moves, elapsed, (double)total_moves / elapsed);

    return moves[best_index];
}