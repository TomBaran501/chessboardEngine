#include "ai.h"
#include <limits.h>

SearchInfo info = {0};

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

static int quiescence_search(Chessboard *board, int alpha, int beta)
{
    int stand_pat = evaluate_position(board);
    info.nb_positions_evaluated += 1;

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
        int score = -quiescence_search(board, -beta, -alpha);
        unplay_move(board, moves[i]);

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}

// ==================== Alpha-Beta ====================

int alpha_beta(Chessboard *board, int depth, int alpha, int beta)
{
    if (depth == 0)
        return quiescence_search(board, alpha, beta);

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
        int score = -alpha_beta(board, depth - 1, -beta, -alpha);
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

static int get_score(Chessboard *board, Move move, int depth)
{
    play_move(board, move);
    int score;
    
    if (depth == 0)
        score = evaluate_position(board); // Potentiellement quiecence search au lieu de evaluate mais probablement pas d'impact
    else
        score = -alpha_beta(board, depth - 1, -INFINI, INFINI);
    
    unplay_move(board, move);
    return score;
}

static ScoredMove search_best_move(Chessboard *board, int depth)
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
    best.score = get_score(board, legal_moves[0], depth);

    for (int i = 1; i < nbmoves; i++)
    {
        int score = get_score(board, legal_moves[i], depth);

        //print_move(&legal_moves[i]);
        //printf("score: %i\n", score);

        if (score > best.score)
        {
            best.score = score;
            best.move = legal_moves[i];
        }
    }

    //printf("nb_pos: %i, score: %i\n", info.nb_positions_evaluated, best.score);

    return best;
}

Move get_best_move(Chessboard board)
{
    info.nb_positions_evaluated = 0;
    ScoredMove best_move = search_best_move(&board, 3);
    return best_move.move;
}