#include "ai.h"
#include <limits.h>

// ==================== Move Ordering ====================

static int get_move_score(Chessboard *board, const Move *move)
{
    int score = 0;

    if (move->promotion_flag == PROMOTION_Q)
        score += QUEEN_VALUE;

    if (move->piece_taken != NONE)
        score += get_piece_value(board, move->to) * 10 - get_piece_value(board, move->from);

    return score;
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

// ==================== Quiescence Search ====================

static int quiescence_search(Chessboard *board, int alpha, int beta, SearchInfo *info)
{
    int stand_pat = evaluate_position(board);
    info->nb_positions_evaluated += 1;

    if (stand_pat >= beta)
        return beta;
    if (stand_pat > alpha)
        alpha = stand_pat;

    Move moves[250];
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

// ==================== Alpha-Beta ====================

int alpha_beta(Chessboard *board, int depth, int alpha, int beta, SearchInfo *info)
{
    if (depth == 0)
        return quiescence_search(board, alpha, beta, info);

    Move moves[250];
    int count = get_all_legal_moves(board, moves);

    if (count == 0)
    {
        if (is_check(board))
            return board->white_to_play ? -MAT + depth : MAT - depth;
        return 0;
    }

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

// ==================== Root Search ====================

static int get_score(Chessboard *board, Move move, int depth, SearchInfo *info)
{
    play_move(board, move);
    int score;
    
    if (depth == 0)
        score = evaluate_position(board); // Potentiellement quiecence search au lieu de evaluate mais probablement pas d'impact
    else
        score = -alpha_beta(board, depth - 1, -INFINI, INFINI, info);
    
    unplay_move(board, move);
    return score;
}

static ScoredMove search_best_move(Chessboard *board, int depth, SearchInfo *info)
{
    Move legal_moves[250];
    int nbmoves = get_all_legal_moves(board, legal_moves);
    
    if (nbmoves == 0)
    {
        ScoredMove result = {0};
        return result;
    }

    ScoredMove best;
    best.move = legal_moves[0];
    best.score = get_score(board, legal_moves[0], depth, info);

    for (int i = 1; i < nbmoves; i++)
    {
        int score = get_score(board, legal_moves[i], depth, info);

        if (score > best.score)
        {
            best.score = score;
            best.move = legal_moves[i];
        }
    }

    return best;
}

SearchInfo get_best_move(Chessboard board)
{
    SearchInfo info = {0};
    info.nb_positions_evaluated = 0;
    info.depth = 3;
    info.log[0] = '\0';

    info.move = search_best_move(&board, info.depth, &info);
    snprintf(info.log, SEARCH_LOG_SIZE, "depth=%d evaluations=%d", info.depth, info.nb_positions_evaluated);
    return info;
}