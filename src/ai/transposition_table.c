#include "transposition_table.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

TTStats tt_stats = {0};

#include <stdio.h>
#include <string.h>

void get_string_stat_tt(char *buffer)
{
    double hit_rate = 0.0;
    double fill_rate = 0.0;

    if (tt_stats.probes > 0)
        hit_rate = (double)tt_stats.hits / (double)tt_stats.probes;

    fill_rate = (double)tt_stats.stores / (double)TT_SIZE;

    snprintf(buffer, TT_STATS_STRING_SIZE,
             "TT | probes=%lu hits=%lu (%.1f%%) cutoffs=%lu | "
             "stores=%lu overwrites=%lu | fill=%.1f%%",
             tt_stats.probes,
             tt_stats.hits,
             hit_rate * 100.0,
             tt_stats.cutoffs,
             tt_stats.stores,
             tt_stats.overwrites,
             fill_rate * 100.0);
}

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

    if (entry->key != key || depth >= entry->depth) // Suspect
    {
        if (entry->key != 0)
            tt_stats.overwrites++;
        else
            tt_stats.stores++;

        entry->key = key;
        entry->depth = depth;
        entry->score = score;
        entry->flag = flag;
        entry->best_move = best_move;
        tt_stats.overwrites++;
    }
}

bool tt_probe(TranspositionTable *tt, ZobristKey key, int depth, int alpha, int beta, int *score, Move *best_move)
{
    uint64_t idx = tt_index(tt, key);
    TTEntry *entry = &tt->entries[idx];
    tt_stats.probes++;

    if (entry->key != key)
        return false;

    *best_move = entry->best_move;
    tt_stats.hits++;

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