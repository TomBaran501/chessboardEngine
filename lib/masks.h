#ifndef MASKS_H
#define MASKS_H

#include <stdint.h>
#include "bitboard.h"

extern uint64_t masks_king_moves[64];

void init_bitboards();

void init_king_masks();

#endif
