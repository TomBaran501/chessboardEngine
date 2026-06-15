#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdint.h>
#include <stdbool.h>

#include "chessboard/hashtables/hash_table.h"
#include "move.h"

#define TT_SIZE (1 << 20) /* ~1 million entrées */

typedef enum
{
    TT_EXACT,
    TT_LOWERBOUND,
    TT_UPPERBOUND
} TTFlag;

typedef struct
{
    ZobristKey key;

    int depth;
    int score;

    TTFlag flag;

    Move best_move;

} TTEntry;

typedef struct
{
    TTEntry *entries;
    uint64_t size;
} TranspositionTable;

void tt_init(TranspositionTable *tt);
void tt_destroy(TranspositionTable *tt);

void tt_clear(TranspositionTable *tt);

void tt_store(
    TranspositionTable *tt,
    ZobristKey key,
    int depth,
    int score,
    TTFlag flag,
    Move best_move);

bool tt_probe(
    TranspositionTable *tt,
    ZobristKey key,
    int depth,
    int alpha,
    int beta,
    int *score,
    Move *best_move);

#endif