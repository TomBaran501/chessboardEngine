#ifndef MASKS_H
#define MASKS_H

#include <stdint.h>
#include "bitboard.h"

#define BLACK 1
#define WHITE 0

extern uint64_t masks_king_moves[64];
extern uint64_t masks_pawn_captures[2][64];

void init_bitboards();

void init_king_masks();
void init_pawn_capture_masks();

#endif
