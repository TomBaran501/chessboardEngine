#include <stdint.h>
#include "chessrules.h"

uint64_t handle_pawn_moves(int pos_piece, Chessboard *board)
{
    uint64_t from_bitboard = create_1bit_board(pos_piece);
    if (!(from_bitboard & board->pawns))
    {
        return 0;
    }

    int color = (board->occupied_white & from_bitboard) ? WHITE : BLACK;
    int direction = (color == WHITE) ? -1 : 1;

    if (masks_pawn_moves[color][pos_piece] & (board->occupied_black | board->occupied_white))
    {
        return create_1bit_board(pos_piece + direction * 8);
    }
    else
    {
        return masks_pawn_moves[color][pos_piece];
    }
}