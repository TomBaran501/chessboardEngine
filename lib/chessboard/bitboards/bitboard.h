#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
#include <stdio.h>

#define count_bits(bitboard) __builtin_popcountll(bitboard)
#define get_lsb_index(bitboard) __builtin_ctzll(bitboard)

uint64_t create_1bit_board(int exponent);
void print_bitboard(uint64_t bitboard);
uint64_t set_bit(uint64_t bitboard, int square);
uint64_t pop_bit(uint64_t bb);
unsigned int get_random_number(void);
uint64_t get_random_bitboard(void);
uint64_t line_mask(int sq1, int sq2);
void init_diag_masks(void);

#endif