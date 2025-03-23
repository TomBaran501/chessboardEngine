#include "masks.h"

uint64_t masks_king_moves[BOARD_SIZE];
uint64_t masks_knight_moves[64];
uint64_t masks_pawn_captures[2][BOARD_SIZE];
uint64_t masks_pawn_moves[2][BOARD_SIZE];

void init_king_masks()
{
    uint64_t bitmask;
    for (int num_case = 0; num_case < BOARD_SIZE; num_case++)
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

void init_pawn_capture_masks()
{
    uint64_t bitmask_white;
    uint64_t bitmask_black;
    for (int num_case = 8; num_case < 56; num_case++)
    {
        bitmask_white = 0;
        bitmask_white |= (num_case % 8 == 0) ? 0 : create_1bit_board(num_case - 9);
        bitmask_white |= (num_case % 8 == 7) ? 0 : create_1bit_board(num_case - 7);

        bitmask_black = 0;
        bitmask_black |= (num_case % 8 == 0) ? 0 : create_1bit_board(num_case + 7);
        bitmask_black |= (num_case % 8 == 7) ? 0 : create_1bit_board(num_case + 9);

        masks_pawn_captures[WHITE][num_case] = bitmask_white;
        masks_pawn_captures[BLACK][num_case] = bitmask_black;
    }
}

void init_pawn_moves_masks()
{
    uint64_t bitmask_white;
    uint64_t bitmask_black;
    for (int num_case = 8; num_case < 56; num_case++)
    {
        bitmask_white = 0;
        bitmask_white |= create_1bit_board(num_case - 8);
        bitmask_white |= (num_case / 8 == 6) ? create_1bit_board(num_case - 16) : 0;

        bitmask_black = 0;
        bitmask_black |= create_1bit_board(num_case + 8);
        bitmask_black |= (num_case / 8 == 1) ? create_1bit_board(num_case + 16) : 0;

        masks_pawn_moves[WHITE][num_case] = bitmask_white;
        masks_pawn_moves[BLACK][num_case] = bitmask_black;
    }
}

void init_knight_masks()
{
    uint64_t bitmask;
    for (int num_case = 0; num_case < BOARD_SIZE; num_case++)
    {
        bitmask = 0;

        if (num_case % 8 > 1)
        {
            bitmask |= (num_case < 56) ? create_1bit_board(num_case + 6) : 0;
            bitmask |= (num_case > 7) ? create_1bit_board(num_case - 10) : 0;
        }
        if (num_case % 8 > 0)
        {
            bitmask |= (num_case < 48) ? create_1bit_board(num_case + 15) : 0;
            bitmask |= (num_case > 15) ? create_1bit_board(num_case - 17) : 0;
        }
        if (num_case % 8 < 7)
        {
            bitmask |= (num_case < 48) ? create_1bit_board(num_case + 17) : 0;
            bitmask |= (num_case > 15) ? create_1bit_board(num_case - 15) : 0;
        }
        if (num_case % 8 < 6)
        {
            bitmask |= (num_case < 56) ? create_1bit_board(num_case + 10) : 0;
            bitmask |= (num_case > 7) ? create_1bit_board(num_case - 6) : 0;
        }

        masks_knight_moves[num_case] = bitmask;
    }
}
