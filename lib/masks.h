#ifndef MASKS_H
#define MASKS_H

#include <stdint.h>
#include "bitboard.h"

#define BLACK 1
#define WHITE 0
#define BOARD_SIZE 64

extern uint64_t masks_king_moves[64];
extern uint64_t masks_knight_moves[64];
extern uint64_t masks_pawn_captures[2][64];
extern uint64_t masks_pawn_moves[2][64];

/// @brief Cette fonction permet d'initialiser les bitsmasks qui vont être un précalcul
/// pour tout les coups possibles pour toutes les pieces. Ces coups sont stockés sous forme de bitboards
/// en tant que variable globale.
void init_bitboards();
uint64_t get_bishop_attacks(int square, uint64_t occupancy);
uint64_t get_rook_attacks(int square, uint64_t occupancy);

#endif
