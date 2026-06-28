#include "transposition_table.h"

TTStats tt_stats = {0};
uint8_t tt_current_age = 0;


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

static inline int score_from_tt(int score, int ply)
{
    if (score > MAT - 1000)
        return score - ply;

    if (score < -MAT + 1000)
        return score + ply;

    return score;
}

static inline int score_to_tt(int score, int ply)
{
    if (score > MAT - 1000)
        return score + ply + 1;
    if (score < -MAT + 1000)
        return score - ply - 1;
    return score;
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

void tt_store(TranspositionTable *tt, ZobristKey key, int depth, int ply,
              int score, TTFlag flag, Move best_move)
{
    uint64_t idx = tt_index(tt, key);
    TTEntry *entry = &tt->entries[idx];

    bool is_old = entry->age != tt_current_age;
    bool same_key = entry->key == key;

    if (!same_key || is_old || depth >= entry->depth)
    {
        entry->key = key;
        entry->depth = depth;
        entry->score = score_to_tt(score, ply);
        entry->flag = flag;
        entry->best_move = best_move;
        entry->age = tt_current_age;
    }
}

bool tt_probe(TranspositionTable *tt, ZobristKey key, int depth, int ply, int alpha, int beta, int *score, Move *best_move)
{
    uint64_t idx = tt_index(tt, key);
    TTEntry *entry = &tt->entries[idx];
    tt_stats.probes++;

    if (entry->key != key)
        return false;

    *best_move = entry->best_move;

    if (entry->age != tt_current_age || entry->depth < depth)
        return false;

    tt_stats.hits++;
    *score = score_from_tt(entry->score, ply);

    switch (entry->flag)
    {
    case TT_EXACT:
        return true;

    case TT_LOWERBOUND:
        if (*score >= beta)
            return true;
        break;

    case TT_UPPERBOUND:
        if (*score <= alpha)
            return true;
        break;
    }

    return false;
}