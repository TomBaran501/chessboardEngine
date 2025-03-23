#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"

#include "stdlib.h"

int main()
{
    init_pawn_capture_masks();
    print_bitboard(masks_pawn_captures[0][16]);
    print_bitboard(masks_pawn_captures[1][16]);
    print_bitboard(masks_pawn_captures[0][36]);
    print_bitboard(masks_pawn_captures[1][36]);
    print_bitboard(masks_pawn_captures[0][7]);
    print_bitboard(masks_pawn_captures[0][7]);
}