#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"

#include "stdlib.h"

int main()
{
    init_king_masks();
    print_bitboard(masks_king_moves[19]);
    print_bitboard(masks_king_moves[3]);
    print_bitboard(masks_king_moves[56]);
    print_bitboard(masks_king_moves[15]);

}