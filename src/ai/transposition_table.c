#include "transposition_table.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static inline uint64_t tt_index(const TranspositionTable *tt, ZobristKey key)
{
    return key % tt->size;
}

void tt_init(TranspositionTable *tt)
{
    tt->size = TT_SIZE;
    tt->entries = calloc(tt->size, sizeof(TTEntry));
}

void tt_destroy(TranspositionTable *tt)
{
    free(tt->entries);
    tt->entries = NULL;
    tt->size = 0;
}

void tt_clear(TranspositionTable *tt)
{
    memset(tt->entries, 0, tt->size * sizeof(TTEntry));
}

void tt_store(TranspositionTable *tt, ZobristKey key, int depth, int score, TTFlag flag, Move best_move)
{
    uint64_t idx = tt_index(tt, key);
    TTEntry *entry = &tt->entries[idx];

    if (entry->key != key || depth >= entry->depth)
    {
        entry->key = key;
        entry->depth = depth;
        entry->score = score;
        entry->flag = flag;
        entry->best_move = best_move;
    }
}

bool tt_probe(TranspositionTable *tt, ZobristKey key, int depth, int alpha, int beta, int *score, Move *best_move)
{
    uint64_t idx = tt_index(tt, key);
    TTEntry *entry = &tt->entries[idx];

    if (entry->key != key)
        return false;

    *best_move = entry->best_move;

    if (entry->depth < depth)
        return false;

    switch (entry->flag)
    {
    case TT_EXACT:
        *score = entry->score;
        return true;

    case TT_LOWERBOUND:
        if (entry->score >= beta)
        {
            *score = entry->score;
            return true;
        }
        break;

    case TT_UPPERBOUND:
        if (entry->score <= alpha)
        {
            *score = entry->score;
            return true;
        }
        break;
    }

    return false;
}