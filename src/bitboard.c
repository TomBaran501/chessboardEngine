#include "bitboard.h"


uint64_t create_1bit_board(uint64_t exponent) {
    uint64_t base = 2;
    uint64_t result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

void print_bitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; rank--) { 
        for (int file = 0; file < 8; file++) { 
            int square = rank * 8 + file;  
            printf("%c ", (bitboard & (1ULL << square)) ? '1' : '.');
        }
        printf("\n");
    }
    printf("\n");
}