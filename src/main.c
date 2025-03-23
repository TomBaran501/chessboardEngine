#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"

#include "stdlib.h"

int main()
{
    init_pawn_moves_masks();
    print_bitboard(masks_pawn_moves[0][15]);
    print_bitboard(masks_pawn_moves[1][15]);
    print_bitboard(masks_pawn_moves[0][36]);
    print_bitboard(masks_pawn_moves[1][36]);
    print_bitboard(masks_pawn_moves[0][7]);
    print_bitboard(masks_pawn_moves[1][7]);
}