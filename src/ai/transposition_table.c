#include "transposition_table.h"

TTStats tt_stats = {0};
uint8_t tt_current_age = 0;

void get_string_stat_tt(char *buffer)
{
    unsigned long probes = atomic_load(&tt_stats.probes);
    unsigned long key_hits = atomic_load(&tt_stats.key_hits);
    unsigned long depth_hits = atomic_load(&tt_stats.depth_hits);
    unsigned long cutoffs = atomic_load(&tt_stats.cutoffs);
    unsigned long stores = atomic_load(&tt_stats.stores);
    unsigned long overwrites = atomic_load(&tt_stats.overwrites);

    snprintf(buffer, TT_STATS_STRING_SIZE,
             "TT | probes=%lu key_hits=%lu (%.1f%%) depth_hits=%lu (%.1f%%) cutoffs=%lu (%.1f%%) | "
             "stores=%lu overwrites=%lu | fill=%.1f%%",
             probes,
             key_hits, probes ? (double)key_hits / probes * 100.0 : 0.0,
             depth_hits, probes ? (double)depth_hits / probes * 100.0 : 0.0,
             cutoffs, probes ? (double)cutoffs / probes * 100.0 : 0.0,
             stores,
             overwrites,
             (double)stores / TT_SIZE * 100.0);
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

    if (tt->entries == NULL)
    {
        fprintf(stderr, "TT alloc failed for %lu entries (%lu MB)\n",
                tt->size, tt->size * sizeof(TTEntry) / (1024 * 1024));
        exit(1);
    }

    fprintf(stderr, "TT initialized: %lu entries (%lu MB)\n",
            tt->size, tt->size * sizeof(TTEntry) / (1024 * 1024));
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
    bool same_key = (entry->key ^ (ZobristKey)entry->score) == key;

    if (!same_key || is_old || depth >= entry->depth)
    {
        if (entry->key != 0)
            atomic_fetch_add(&tt_stats.overwrites, 1);
        else
            atomic_fetch_add(&tt_stats.stores, 1);

        int32_t stored_score = score_to_tt(score, ply);

        entry->score = stored_score;
        entry->depth = depth;
        entry->flag = flag;
        entry->best_move = best_move;
        entry->age = tt_current_age;
        entry->key = key ^ (ZobristKey)stored_score;
    }
}

bool tt_probe(TranspositionTable *tt, ZobristKey key, int depth, int ply, int alpha, int beta, int *score, Move *best_move)
{
    uint64_t idx = tt_index(tt, key);
    TTEntry *entry = &tt->entries[idx];
    atomic_fetch_add(&tt_stats.probes, 1);

    // Vérifier la clé XOR score — détecte les écritures partielles
    if ((entry->key ^ (ZobristKey)entry->score) != key)
        return false;

    atomic_fetch_add(&tt_stats.key_hits, 1);

    *best_move = entry->best_move;

    if (entry->age != tt_current_age || entry->depth < depth)
        return false;

    atomic_fetch_add(&tt_stats.depth_hits, 1);
    *score = score_from_tt(entry->score, ply);

    switch (entry->flag)
    {
    case TT_EXACT:
        atomic_fetch_add(&tt_stats.cutoffs, 1);
        return true;

    case TT_LOWERBOUND:
        if (*score >= beta)
        {
            atomic_fetch_add(&tt_stats.cutoffs, 1);
            return true;
        }
        break;

    case TT_UPPERBOUND:
        if (*score <= alpha)
        {
            atomic_fetch_add(&tt_stats.cutoffs, 1);
            return true;
        }
        break;
    }

    return false;
}