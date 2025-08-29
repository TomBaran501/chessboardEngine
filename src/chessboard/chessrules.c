#include <stdint.h>
#include "chessrules.h"

int castling_pos[2][2] = {{62, 58}, {6, 2}};
uint64_t castling_empty_squares[2][2] = {{(1ULL << 61) + (1ULL << 62), (1ULL << 58) + (1ULL << 59) + (1ULL << 57)},
                                         {(1ULL << 6) + (1ULL << 5), (1ULL << 1) + (1ULL << 2) + (1ULL << 3)}};

uint64_t handle_pawn_moves(int pos_piece, Chessboard *board)
{
    uint64_t from_bitboard = create_1bit_board(pos_piece);
    if (!(from_bitboard & board->pawns))
        return 0;

    int color = (board->occupied_white & from_bitboard) ? WHITE : BLACK;
    int direction = (color == WHITE) ? -1 : 1;

    if (masks_pawn_moves[color][pos_piece] & (board->occupied_black | board->occupied_white))
        return create_1bit_board(pos_piece + direction * 8) & (board->occupied_black | board->occupied_white) ? 0 : create_1bit_board(pos_piece + direction * 8);

    else
        return masks_pawn_moves[color][pos_piece];
}


//Attention!!! rajouter la vérification des échecs sur les cases ou passent le roi
uint64_t handle_roque_moves(int pos_piece, Chessboard *board)
{
    uint64_t from_bitboard = create_1bit_board(pos_piece);
    if (!(from_bitboard & board->kings))
        return 0;

    int color = (board->occupied_white & from_bitboard) ? WHITE : BLACK;
    uint64_t mask_roque_moves = 0;
    uint64_t occupied_squares = board->occupied_black | board->occupied_white;

    if (((castling_empty_squares[color][0] & occupied_squares) == 0) && (board->castling & create_1bit_board(castling_pos[color][0])))
        mask_roque_moves |= create_1bit_board(castling_pos[color][0]);

    if (((castling_empty_squares[color][1] & occupied_squares) == 0) && (board->castling & create_1bit_board(castling_pos[color][1])))
        mask_roque_moves |= create_1bit_board(castling_pos[color][1]);

    return mask_roque_moves;
}
