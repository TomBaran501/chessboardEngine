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
    *nbcoups += 1;
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

int alphabeta(Chessboard *board, int depth, int alpha, int beta, unsigned long long *nbcoups, unsigned long long *nb_cuts_tt)
{
    uint64_t key = compute_hash(board);
    TTEntry *entry = tt_probe(key, depth, alpha, beta);

    if (entry)
    {
        *nb_cuts_tt += 1;
        return entry->score;
    }

    if (depth == 0)
        return quiescence_search(board, alpha, beta, nbcoups, 0);

    Move moves[250];
    int nbmoves = getalllegalmoves(board, moves);

    if (nbmoves == 0)
    {
        if (is_check(board))
            return -MAT - depth; // mat rapide = meilleur

        else
            return 0;
    }

    sort_moves_heuristic(moves, nbmoves);
    int value = -INFINI;

    for (int i = 0; i < nbmoves; i++)
    {
        play_move(board, moves[i]);
        int score = -alphabeta(board, depth - 1, -beta, -alpha, nbcoups, nb_cuts_tt);
        unplay_move(board, moves[i]);

        if (score > value)
            value = score;

        if (value > alpha)
            alpha = value;

        if (alpha >= beta)
            return alpha;
    }

    TTFlag flag;

    if (value <= alpha)
        flag = TT_FLAG_UPPERBOUND;

    else if (value >= beta)
        flag = TT_FLAG_LOWERBOUND;

    else
        flag = TT_FLAG_EXACT;

    tt_store(key, depth, value, flag);
    return value;
}

int iterative_deepening(Chessboard *board, unsigned long long *nbcoups, int time_limit_ms, int *true_depth, unsigned long long *nb_cuts_tt)
{
    Move moves[250];
    ScoredMove scored_moves[250];
    int nbmoves = getalllegalmoves(board, moves);

    if (nbmoves == 0)
    {
        if (is_check(board))
            return -MAT - MAX_DEPTH;
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
    Move pv_move = {0}; // meilleur coup trouvé à la profondeur précédente

    for (int d = 1; d <= MAX_DEPTH; d++)
    {
        // On met le meilleur coup potentiel en tête
        if (pv_move.from || pv_move.to)
        {
            for (int i = 0; i < nbmoves; i++)
            {
                if (scored_moves[i].move.from == pv_move.from && scored_moves[i].move.to == pv_move.to)
                {
                    ScoredMove tmp = scored_moves[0];
                    scored_moves[0] = scored_moves[i];
                    scored_moves[i] = tmp;
                    break;
                }
            }
        }

        // aspiration window
        int window = 100;
        int alpha = (d == 1) ? -INFINI : best_score - window;
        int beta = (d == 1) ? INFINI : best_score + window;

        int attempt = 0;
        const int MAX_ATTEMPTS = 4;
        int tried_full = 0;
        int root_score = -INFINI;

        while (1)
        {
            int local_best = -INFINI;
            int cur_alpha = alpha;
            int cur_beta = beta;

            for (int i = 0; i < nbmoves; i++)
            {
                if (elapsed_ms(start) > time_limit_ms)
                {
                    *true_depth = d - 1;
                    return best_score;
                }

                play_move(board, scored_moves[i].move);
                int val = -alphabeta(board, d - 1, -cur_beta, -cur_alpha, nbcoups, nb_cuts_tt);
                unplay_move(board, scored_moves[i].move);

                scored_moves[i].score = val;

                if (val > local_best)
                    local_best = val;

                if (val > cur_alpha)
                    cur_alpha = val;

                if (cur_alpha >= cur_beta)
                    break; // cutoff racine
            }

            root_score = local_best;

            // 1) fail-low
            if (root_score <= alpha)
            {
                attempt++;
                if (tried_full || attempt >= MAX_ATTEMPTS)
                {
                    alpha = -INFINI;
                    beta = INFINI;
                    tried_full = 1;
                }
                else
                {
                    window = window * 2 + 1;
                    alpha = root_score - window;
                    beta = root_score + window;
                }
                continue;
            }
            // 2) fail-high
            else if (root_score >= beta)
            {
                attempt++;
                if (tried_full || attempt >= MAX_ATTEMPTS)
                {
                    alpha = -INFINI;
                    beta = INFINI;
                    tried_full = 1;
                }
                else
                {
                    window = window * 2 + 1;
                    alpha = root_score - window;
                    beta = root_score + window;
                }
                continue;
            }

            // 3) résultat valide
            break;
        }

        // ordering pour la prochaine profondeur
        qsort(scored_moves, nbmoves, sizeof(ScoredMove), compare_moves_desc);

        best_score = scored_moves[0].score;
        pv_move = scored_moves[0].move; // on garde le PV
    }

    *true_depth = MAX_DEPTH;
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
    unsigned long long nb_cuts_tt = 0;
    task->score = -iterative_deepening(&local_board, &nbcoups, 300, &true_depth, &nb_cuts_tt);
    task->nbmoves = nbcoups;
    task->true_depth = true_depth + 1;
    task->nb_cuts_tt = nb_cuts_tt;

    return NULL;
}

Move get_best_move(Chessboard board)
{
    Move moves[250];
    int nbmoves = getalllegalmoves(&board, moves);
    int total_moves = 0;
    double depth = 0;
    int nb_cuts_tt = 0;

    pthread_t threads[250];
    ThreadTask tasks[250];

    struct timespec t_start, t_end;
    if (clock_gettime(1, &t_start) != 0)
        perror("clock_gettime start");

    // Lancer un thread par coup légal
    for (int i = 0; i < nbmoves; i++)
    {
        tasks[i].board = board;
        tasks[i].move = moves[i];
        tasks[i].score = -INFINI;
        tasks[i].nbmoves = 0;
        tasks[i].true_depth = 0;
        tasks[i].nb_cuts_tt = 0;

        pthread_create(&threads[i], NULL, thread_worker_ai, &tasks[i]);
    }

    // Attendre les threads
    for (int i = 0; i < nbmoves; i++)
    {
        pthread_join(threads[i], NULL);
        total_moves += tasks[i].nbmoves;
        depth += tasks[i].true_depth;
        nb_cuts_tt += tasks[i].nb_cuts_tt;
    }

    depth /= nbmoves;

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
    printf(" is the best move with score: %d at average depth %f evaluating %i positions... en %.6f secondes... %.0f pos/s    nb_cuts_tt: %i\n\n",
           best_score, depth, total_moves, elapsed, (double)total_moves / elapsed, nb_cuts_tt);
    return moves[best_index];
}

void initialise_ai(int color_ai)
{
    initialize_tables(color_ai);
    init_zobrist();
}
