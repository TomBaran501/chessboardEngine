#include "masks.h"


uint64_t masks_king_moves[64];


void init_king_masks()
{
    uint64_t bitmask;
    for (int num_case = 0; num_case < 64; num_case++)
    {
        bitmask = 0;
        if (num_case % 8 != 0)
        {
            bitmask |= create_1bit_board(num_case - 1);
            bitmask |= (num_case > 55) ? 0 : create_1bit_board(num_case + 7);
            bitmask |= (num_case < 8) ? 0 : create_1bit_board(num_case - 9);
        }
        if (num_case % 8 != 7)
        {
            bitmask |= create_1bit_board(num_case + 1);
            bitmask |= (num_case > 55) ? 0 : create_1bit_board(num_case + 9);
            bitmask |= (num_case < 8) ? 0 : create_1bit_board(num_case - 7);
        }

        bitmask |= (num_case < 8) ? 0 : create_1bit_board(num_case - 8);
        bitmask |= (num_case > 55) ? 0 : create_1bit_board(num_case + 8);

        masks_king_moves[num_case] = bitmask;
    }
}