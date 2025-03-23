#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
#include <stdio.h>

uint64_t create_1bit_board(int exponent);
void print_bitboard(uint64_t bitboard);

#endif