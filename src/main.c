#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"

#include "stdlib.h"

int main()
{
    init_knight_masks();
    print_bitboard(masks_knight_moves[63]);
    print_bitboard(masks_knight_moves[19]);
    print_bitboard(masks_knight_moves[26]);
    print_bitboard(masks_knight_moves[36]);
    print_bitboard(masks_knight_moves[48]);
    print_bitboard(masks_knight_moves[59]);
}