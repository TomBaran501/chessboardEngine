#ifndef MASKS_H
#define MASKS_H

#include <stdint.h>
#include "bitboard.h"

#define BLACK 1
#define WHITE 0

extern uint64_t masks_king_moves[64];
extern uint64_t masks_pawn_captures[2][64];
extern uint64_t masks_pawn_moves[2][64];

/// @brief Cette fonction permet d'initialiser les bitsmasks qui vont être un précalcul
/// pour tout les coups possibles pour toutes les pieces. Ces coups sont stockés sous forme de bitboards
///en tant que variable globale.
void init_bitboards();

void init_king_masks();
void init_pawn_capture_masks();
void init_pawn_moves_masks();

#endif
