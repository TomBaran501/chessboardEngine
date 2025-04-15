#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"

#include "stdlib.h"

int main()
{
    init_bitboards();
    int64_t occ= create_1bit_board(9);
    print_bitboard(get_bishop_attacks(0,occ));
    return 0;
}