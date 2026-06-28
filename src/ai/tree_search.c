#include "tree_search.h"

TranspositionTable global_tt;

static bool search_should_stop(const SearchInfo *info)
{
    return info != NULL && info->stop_search != NULL && atomic_load(info->stop_search);
}

// ==================== Helper Functions ====================

/// @brief On regarde en premier les captures, les promotions et les meilleurs coups des recherches précédentes, car il y a de fortes chances que ce soit le meilleur coup.
static int get_move_score(Chessboard *board, const Move *move)
{
    int score = 0;

    if (move->promotion_flag == PROMOTION_Q)
        score += QUEEN_VALUE * 10;

    if (move->piece_taken != NONE)
        score += get_piece_value(board, move->to) * 10;

    return score;
}

static bool is_tt_move(const Move *move, Move tt_move)
{
    return tt_move.from == move->from &&
           tt_move.to == move->to &&
           tt_move.promotion_flag == move->promotion_flag;
}

static int get_game_state(Chessboard *board, int ply)
{
    if (is_check(board))
        return -MAT + ply;
    return 0;
}

static void order_moves(Chessboard *board, Move *moves, int count,
                        SearchInfo *info, Move tt_move)
{
    if (count <= 1)
        return;

    int scores[MAX_LEGAL_MOVES];

    // 1. pré-calc scores (IMPORTANT: évite recalcul multiple)
    for (int i = 0; i < count; i++)
    {
        if (search_should_stop(info))
            return;

        if (is_tt_move(&moves[i], tt_move))
            scores[i] = 1000000;

        else
            scores[i] = get_move_score(board, &moves[i]);
    }

    // 2. FORCER TT MOVE EN POSITION 0
    int tt_index = -1;
    for (int i = 0; i < count; i++)
    {
        if (scores[i] == 1000000)
        {
            tt_index = i;
            break;
        }
    }

    if (tt_index > 0)
    {
        Move tmp = moves[0];
        moves[0] = moves[tt_index];
        moves[tt_index] = tmp;

        int stmp = scores[0];
        scores[0] = scores[tt_index];
        scores[tt_index] = stmp;
    }

    // 3. tri simple sur le reste (insertion sort stable mais propre)
    for (int i = 1; i < count; i++)
    {
        Move key_move = moves[i];
        int key_score = scores[i];
        int j = i - 1;

        while (j >= 1 && scores[j] < key_score)
        {
            moves[j + 1] = moves[j];
            scores[j + 1] = scores[j];
            j--;
        }

        moves[j + 1] = key_move;
        scores[j + 1] = key_score;
    }
}

// ==================== Tree-Searches ====================

// Ajouter les echecs à vérifier en plus des captures
static int quiescence_search(Chessboard *board, int alpha, int beta, SearchInfo *info)
{
    int stand_pat = evaluate_position(board);
    info->nb_positions_evaluated += 1;

    if (search_should_stop(info))
        return stand_pat;

    if (stand_pat >= beta)
        return beta;
    if (stand_pat > alpha)
        alpha = stand_pat;

    Move moves[MAX_LEGAL_MOVES];
    int count = get_all_legal_moves(board, moves);

    for (int i = 0; i < count; i++)
    {
        if (search_should_stop(info))
            break;

        if (moves[i].piece_taken == NONE)
            continue;

        play_move(board, moves[i]);
        int score = -quiescence_search(board, -beta, -alpha, info);
        unplay_move(board, moves[i]);

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

int alpha_beta(Chessboard *board, int depth, int ply, int alpha, int beta, SearchInfo *info)
{
    if (search_should_stop(info))
        return evaluate_position(board);

    int tt_score;
    Move tt_move = {0};
    int original_alpha = alpha;

    if (tt_probe(&global_tt, board->hash, depth, ply,alpha, beta, &tt_score, &tt_move))
    {
        tt_stats.cutoffs++;
        return tt_score;
    }

    if (depth == 0)
        return quiescence_search(board, alpha, beta, info);

    Move moves[MAX_LEGAL_MOVES];
    int count = get_all_legal_moves(board, moves);

    if (count == 0)
        return get_game_state(board, ply);

    order_moves(board, moves, count, info, tt_move);
    int best = -INFINI;
    Move best_move = moves[0];
    bool search_completed = true;
    bool evaluated_any_move = false;

    for (int i = 0; i < count; i++)
    {
        if (search_should_stop(info))
        {
            search_completed = false;
            break;
        }

        int score;
        int gamestate = play_move_check_is_three_fold_repetition(board, moves[i]);
        if (gamestate == DRAW)
            score = 0;
        else
            score = -alpha_beta(board, depth - 1, ply + 1, -beta, -alpha, info);
        unplay_move(board, moves[i]);
        evaluated_any_move = true;

        if (score > best)
        {
            best = score;
            best_move = moves[i];
        }

        if (score > alpha)
            alpha = score;

        if (beta <= alpha)
            break;
    }

    if (!search_completed)
    {
        if (!evaluated_any_move)
            return evaluate_position(board);
        return best;
    }

    TTFlag flag = TT_EXACT;

    if (best <= original_alpha)
        flag = TT_UPPERBOUND;
    else if (best >= beta)
        flag = TT_LOWERBOUND;

    tt_store(&global_tt, board->hash, depth, ply, best, flag, best_move);

    return best;
}
