#include "ai.h"
#include <limits.h>

// Fonction pour trier les coups dans l'ordre décroissant
static void sort_moves(ScoredMove *scored_moves, int nbmoves)
{
    for (int i = 0; i < nbmoves - 1; i++)
    {
        for (int j = 0; j < nbmoves - i - 1; j++)
        {
            if (scored_moves[j].score < scored_moves[j + 1].score)
            {
                ScoredMove temp = scored_moves[j];
                scored_moves[j] = scored_moves[j + 1];
                scored_moves[j + 1] = temp;
            }
        }
    }
}

static int get_ending_score(Chessboard *board, int is_maximizing)
{
    if (is_check(board))
        return is_maximizing ? INT_MIN + 1 : INT_MAX - 1; // Checkmate
    return 0;                                             // Stalemate
}

/* Helper: simple in-place partition to put capture moves first (heuristic d'ordre) */
static void order_moves_by_capture(Move moves[], int nbmoves)
{
    if (nbmoves <= 1)
        return;
    int i = 0, j = nbmoves - 1;
    while (i < j)
    {
        while (i < nbmoves && moves[i].piece_taken)
            i++;
        while (j >= 0 && !moves[j].piece_taken)
            j--;
        if (i < j)
        {
            Move tmp = moves[i];
            moves[i] = moves[j];
            moves[j] = tmp;
            i++;
            j--;
        }
    }
}

static int alpha_beta_search(Chessboard *board, int depth, int alpha, int beta, int is_maximizing, int nbmoves, Move *legal_moves)
{
    if (is_maximizing)
    {
        int best = INT_MIN;
        for (int i = 0; i < nbmoves; ++i)
        {
            play_move(board, legal_moves[i]);
            int val = alpha_beta(board, depth - 1, alpha, beta, !is_maximizing);
            unplay_move(board, legal_moves[i]);

            if (val > best)
                best = val;
            if (val > alpha)
                alpha = val;
            if (alpha >= beta)
                break; /* beta-cutoff */
        }
        return best;
    }
    else
    {
        int best = INT_MAX;
        for (int i = 0; i < nbmoves; ++i)
        {
            play_move(board, legal_moves[i]);
            int val = alpha_beta(board, depth - 1, alpha, beta, !is_maximizing);
            unplay_move(board, legal_moves[i]);

            if (val < best)
                best = val;
            if (val < beta)
                beta = val;
            if (beta <= alpha)
                break; /* alpha-cutoff */
        }
        return best;
    }
}

int alpha_beta(Chessboard *board, int depth, int alpha, int beta, int is_maximizing)
{
    if (depth == 0)
        return evaluate_position(board);

    Move legal_moves[250];
    int nbmoves = get_all_legal_moves(board, legal_moves);

    if (nbmoves == 0)
        return get_ending_score(board, is_maximizing);

    order_moves_by_capture(legal_moves, nbmoves);

    return alpha_beta_search(board, depth, alpha, beta, is_maximizing, nbmoves, legal_moves);
}

static int get_score(Chessboard board, Move move, int depth)
{
    play_move(&board, move);
    if (depth == 0)
        return evaluate_position(&board);

    return alpha_beta(&board, depth - 1, INT_MIN, INT_MAX, !board.white_to_play);
}

static ScoredMove search_best_move(Chessboard board, int depth)
{
    Move legal_moves[250];
    int nbmoves = get_all_legal_moves(&board, legal_moves);
    ScoredMove scored_moves[nbmoves];

    for (int i = 0; i < nbmoves; i++)
    {
        scored_moves[i].move = legal_moves[i];
        scored_moves[i].score = get_score(board, legal_moves[i], depth);
    }

    sort_moves(scored_moves, nbmoves);
    return scored_moves[0];
}

Move get_best_move(Chessboard board)
{
    ScoredMove best_move = search_best_move(board, 3);
    return best_move.move;
}
