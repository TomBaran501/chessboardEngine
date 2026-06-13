#include "tree_search.h"

// ==================== Helper Functions ====================

/// @brief On regarde en premier les captures et les promotions, car il y a de fortes chances que ce soit le meilleur coup.
static int get_move_score(Chessboard *board, const Move *move)
{
    int score = 0;
    if (move->promotion_flag == PROMOTION_Q)
        score += QUEEN_VALUE;

    if (move->piece_taken != NONE)
        score += get_piece_value(board, move->to) * 10 - get_piece_value(board, move->from);

    return score;
}

static void init_scored_move(int nbmoves, ScoredMove scored_moves[250], Move legal_moves[250])
{
    for (int i = 0; i < nbmoves; i++)
    {
        scored_moves[i].move = legal_moves[i];
        scored_moves[i].score = 0;
    }
}

static int get_game_state(Chessboard *board, int depth)
{
    if (is_check(board))
        return -MAT - depth;
    return 0;
}

static void order_moves(Chessboard *board, Move *moves, int count)
{
    for (int i = 1; i < count; i++)
    {
        Move temp = moves[i];
        int temp_score = get_move_score(board, &moves[i]);
        int j = i - 1;

        while (j >= 0 && get_move_score(board, &moves[j]) < temp_score)
        {
            moves[j + 1] = moves[j];
            j--;
        }
        moves[j + 1] = temp;
    }
}

static void sort_scored_moves(ScoredMove *moves, int count)
{
    for (int i = 1; i < count; i++)
    {
        ScoredMove temp = moves[i];
        int j = i - 1;

        while (j >= 0 && moves[j].score < temp.score)
        {
            moves[j + 1] = moves[j];
            j--;
        }

        moves[j + 1] = temp;
    }
}

// ==================== Tree-Searches ====================

// Ajouter les echecs à vérifier en plus des captures
static int quiescence_search(Chessboard *board, int alpha, int beta, SearchInfo *info)
{
    int stand_pat = evaluate_position(board);
    info->nb_positions_evaluated += 1;

    if (stand_pat >= beta)
        return beta;
    if (stand_pat > alpha)
        alpha = stand_pat;

    Move moves[MAX_LEGAL_MOVES];
    int count = get_all_legal_moves(board, moves);

    for (int i = 0; i < count; i++)
    {
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

int alpha_beta(Chessboard *board, int depth, int alpha, int beta, SearchInfo *info)
{
    if (depth == 0)
        return quiescence_search(board, alpha, beta, info);

    Move moves[MAX_LEGAL_MOVES];
    int count = get_all_legal_moves(board, moves);

    if (count == 0)
        return get_game_state(board, depth);

    order_moves(board, moves, count);
    int best = -INFINI;

    for (int i = 0; i < count; i++)
    {
        play_move(board, moves[i]);
        int score = -alpha_beta(board, depth - 1, -beta, -alpha, info);
        unplay_move(board, moves[i]);

        if (score > best)
            best = score;
        if (score > alpha)
            alpha = score;

        if (beta <= alpha)
            break;
    }

    return best;
}

static int score_move_alpha_beta(Chessboard *board, Move move, int depth, int alpha, int beta, SearchInfo *info)
{
    play_move(board, move);
    int score = -alpha_beta(board, depth - 1, -beta, -alpha, info);
    unplay_move(board, move);
    return score;
}

ScoredMove search_best_move(Chessboard *board, int depth, SearchInfo *info)
{
    Move legal_moves[MAX_LEGAL_MOVES];
    int nbmoves = get_all_legal_moves(board, legal_moves);

    if (nbmoves == 0)
    {
        ScoredMove terminal = {0};
        terminal.score = get_game_state(board, depth);
        return terminal;
    }

    order_moves(board, legal_moves, nbmoves);

    ScoredMove scored_moves[MAX_LEGAL_MOVES];
    init_scored_move(nbmoves, scored_moves, legal_moves);
    ScoredMove best = {0};

    for (int current_depth = 1; current_depth <= depth; current_depth++)
    {
        if (current_depth > 1)
            sort_scored_moves(scored_moves, nbmoves);

        int alpha = -INFINI;

        for (int i = 0; i < nbmoves; i++)
        {
            int score = score_move_alpha_beta(board, scored_moves[i].move, current_depth, alpha, INFINI, info);
            scored_moves[i].score = score;

            if (score > alpha)
                alpha = score;
        }

        sort_scored_moves(scored_moves, nbmoves);
        best.move = scored_moves[0].move;
        best.score = scored_moves[0].score;
    }
    return best;
}
