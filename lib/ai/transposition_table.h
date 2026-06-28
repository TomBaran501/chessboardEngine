#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "chessboard/hashtables/hash_table.h"
#include "move.h"

#define TT_SIZE (1 << 20) /* ~1 million entrées */
#define TT_STATS_STRING_SIZE 128
#define MAT 900000

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
    uint8_t age;

    Move best_move;

} TTEntry;

typedef struct
{
    TTEntry *entries;
    uint64_t size;
} TranspositionTable;

typedef struct
{
    uint64_t probes;
    uint64_t hits;
    uint64_t cutoffs;

    uint64_t stores;
    uint64_t overwrites;

    uint64_t exact_hits;
    uint64_t lower_hits;
    uint64_t upper_hits;
} TTStats;

extern TTStats tt_stats;
extern uint8_t tt_current_age;

void tt_init(TranspositionTable *tt);
void tt_destroy(TranspositionTable *tt);

void tt_clear(TranspositionTable *tt);

void tt_store(
    TranspositionTable *tt,
    ZobristKey key,
    int depth,
    int ply,
    int score,
    TTFlag flag,
    Move best_move);

bool tt_probe(TranspositionTable *tt, ZobristKey key, int depth, int ply, int alpha, int beta, int *score, Move *best_move);

void get_string_stat_tt(char *buffer);

#endif