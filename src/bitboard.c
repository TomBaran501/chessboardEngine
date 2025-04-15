#include "bitboard.h"

unsigned int state = 1884258747;

uint64_t create_1bit_board(int exp)
{
    return (1ULL << exp);
}

uint64_t set_bit(uint64_t bitboard, int square) {
    return bitboard | (1ULL << square);
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

uint64_t pop_bit(uint64_t bb) {
    if (bb == 0) return -1; 
    bb &= bb - 1;
    return bb;
}

unsigned int get_random_number()
{
    unsigned int num = state;
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;
    state = num;
    return num;
}

uint64_t get_random_bitboard()
{
    uint64_t n1, n2, n3, n4;

    n1 = (uint64_t) (get_random_number() & 0xFFFF);
    n2 = (uint64_t) (get_random_number() & 0xFFFF);
    n3 = (uint64_t) (get_random_number() & 0xFFFF);
    n4 = (uint64_t) (get_random_number() & 0xFFFF);

    return n1 | (n2 << 16) | (n2 << 16) | (n3 << 32) | (n4 << 48);
}