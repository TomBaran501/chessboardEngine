#include "ai.h"

struct timespec t_start;
int max_time_ms = 100;

typedef struct
{
    Move best_move;
    int evaluation;
} SearchResult;

/**
 * Recherche Alpha-Beta récursive
 * Retourne une évaluation numérique pour la position courante.
 */
static int alpha_beta(Chessboard *board, int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (depth == 0) // Rajouter une condition sur mat et pat
    {
        return evaluate_position(board);
    }

    Move moves[250];
    int nb_moves = getalllegalmoves(board, moves);

    if (nb_moves == 0)
        return evaluate_position(board);

    if (maximizingPlayer)
    {
        int maxEval = -INFINI;
        for (int i = 0; i < nb_moves; i++)
        {
            Chessboard next_board = *board;
            play_move(&next_board, moves[i]);

            int eval = alpha_beta(&next_board, depth - 1, alpha, beta, false);
            if (eval > maxEval)
                maxEval = eval;

            if (maxEval > alpha)
                alpha = maxEval;

            if (beta <= alpha)
                break; // coupure Beta
        }
        return maxEval;
    }
    else
    {
        int minEval = INFINI;
        for (int i = 0; i < nb_moves; i++)
        {
            Chessboard next_board = *board;
            play_move(&next_board, moves[i]);

            int eval = alpha_beta(&next_board, depth - 1, alpha, beta, true);
            if (eval < minEval)
                minEval = eval;

            if (minEval < beta)
                beta = minEval;

            if (beta <= alpha)
                break; // coupure Alpha
        }
        return minEval;
    }
}

/**
 * Recherche le meilleur coup à une profondeur donnée.
 */
static SearchResult search_best_move_alpha_beta(Chessboard *board, int depth)
{
    Move moves[250];
    int nb_moves = getalllegalmoves(board, moves);

    SearchResult result;
    result.best_move = moves[0];
    result.evaluation = board->white_to_play ? -INFINI : INFINI;

    bool maximizing = board->white_to_play;

    int alpha = -INFINI;
    int beta = INFINI;

    for (int i = 0; i < nb_moves; i++)
    {
        Chessboard next_board = *board;
        play_move(&next_board, moves[i]);

        int eval = alpha_beta(&next_board, depth - 1, alpha, beta, !maximizing);

        if (maximizing && eval > result.evaluation)
        {
            result.evaluation = eval;
            result.best_move = moves[i];
        }
        else if (!maximizing && eval < result.evaluation)
        {
            result.evaluation = eval;
            result.best_move = moves[i];
        }
    }

    return result;
}

Move get_best_move(Chessboard board)
{
    SearchResult best_move = search_best_move_alpha_beta(&board, 5);
    return best_move.best_move;
}
