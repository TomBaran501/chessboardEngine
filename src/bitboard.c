#include "bitboard.h"

uint64_t create_1bit_board(int exp)
{
    uint64_t base = 2;
    long long result = 1;
    while (exp > 0)
    {
        if (exp % 2 == 1)
            result *= base;

        base *= base;
        exp /= 2;
    }
    return result;
}

void print_bitboard(uint64_t bitboard)
{
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            printf("%c ", (bitboard & (1ULL << square)) ? '1' : '.');
        }
        printf("\n");
    }
    printf("\n");
}