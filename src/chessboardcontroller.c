#include "chessboardcontroller.h"

uint64_t getattacks(int piecePos, Chessboard *chessboard)
{
    uint64_t pos = create_1bit_board(piecePos);
    int color = (chessboard->occupied_white & pos) ? WHITE : BLACK;

    if (chessboard->pawns & pos)
        return masks_pawn_moves[color][piecePos];
    else if (chessboard->knights & pos)
        return masks_knight_moves[piecePos];
    else if (chessboard->bishops & pos)
        return get_bishop_attacks(piecePos, (chessboard->occupied_black) & (chessboard->occupied_white));
    else if (chessboard->rooks & pos)
        return get_rook_attacks(piecePos, (chessboard->occupied_black) & (chessboard->occupied_white));
    else if (chessboard->queens & pos)
        return get_bishop_attacks(piecePos, (chessboard->occupied_black) & (chessboard->occupied_white)) & get_rook_attacks(piecePos, (chessboard->occupied_black) & (chessboard->occupied_white));
    else if (chessboard->kings & pos)
        return masks_king_moves[piecePos];
    else
        return 0;
}

MoveList *getlegalmoves(int piecePos, Chessboard *chessboard)
{
    MoveList *moves = createMoveList(10);
    uint64_t attacks = getattacks(piecePos, chessboard);
    int nbmoves = count_bits(attacks);

    for (int m = 0; m < nbmoves; m++)
    {
        Move move;
        initialise_move(&move, piecePos, get_lsb_index(attacks));
        addMove(moves, move);
        attacks = pop_bit(attacks);
    }

    return moves;
}
