#include "transposition_tables.h"
#include <stdlib.h>
#include <string.h>

// Table des entrées (statiquement allouée)
static TTEntry transposition_table[TT_SIZE];

// Clés de Zobrist
static uint64_t zobrist_pieces[12][64]; // 6 types de pièces * 2 couleurs * 64 cases
static uint64_t zobrist_side;           // pour savoir qui doit jouer
static uint64_t zobrist_castling[16];   // états possibles du roque (4 bits)
static uint64_t zobrist_enpassant[8];   // colonnes possibles pour en passant

// Compteurs globaux
static unsigned long long tt_filled = 0;
static unsigned long long tt_replaced = 0;

static uint64_t rand64()
{
    return ((uint64_t)rand() << 48) ^
           ((uint64_t)rand() << 32) ^
           ((uint64_t)rand() << 16) ^
           (uint64_t)rand();
}

void init_zobrist()
{
    srand(1804289383); // seed fixe pour reproductibilité

    for (int piece = 0; piece < 12; piece++)
    {
        for (int square = 0; square < 64; square++)
        {
            zobrist_pieces[piece][square] = rand64();
        }
    }

    zobrist_side = rand64();

    for (int i = 0; i < 16; i++)
    {
        zobrist_castling[i] = rand64();
    }

    for (int i = 0; i < 8; i++)
    {
        zobrist_enpassant[i] = rand64();
    }

    memset(transposition_table, 0, sizeof(transposition_table));
}

static inline int piece_index(int piece_type, int is_white)
{
    return piece_type + (is_white ? 0 : 6);
}

uint64_t compute_hash(const Chessboard *board)
{
    uint64_t hash = 0ULL;

    // Pions
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->pawns & mask)
        {
            if (board->occupied_white & mask)
                hash ^= zobrist_pieces[piece_index(0, 1)][sq];
            else
                hash ^= zobrist_pieces[piece_index(0, 0)][sq];
        }
    }

    // Cavaliers
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->knights & mask)
        {
            if (board->occupied_white & mask)
                hash ^= zobrist_pieces[piece_index(1, 1)][sq];
            else
                hash ^= zobrist_pieces[piece_index(1, 0)][sq];
        }
    }

    // Fous
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->bishops & mask)
        {
            if (board->occupied_white & mask)
                hash ^= zobrist_pieces[piece_index(2, 1)][sq];
            else
                hash ^= zobrist_pieces[piece_index(2, 0)][sq];
        }
    }

    // Tours
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->rooks & mask)
        {
            if (board->occupied_white & mask)
                hash ^= zobrist_pieces[piece_index(3, 1)][sq];
            else
                hash ^= zobrist_pieces[piece_index(3, 0)][sq];
        }
    }

    // Dames
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->queens & mask)
        {
            if (board->occupied_white & mask)
                hash ^= zobrist_pieces[piece_index(4, 1)][sq];
            else
                hash ^= zobrist_pieces[piece_index(4, 0)][sq];
        }
    }

    // Rois
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->kings & mask)
        {
            if (board->occupied_white & mask)
                hash ^= zobrist_pieces[piece_index(5, 1)][sq];
            else
                hash ^= zobrist_pieces[piece_index(5, 0)][sq];
        }
    }

    // Côté au trait
    if (board->white_to_play == 0)
        hash ^= zobrist_side;

    // Roques (4 bits -> max 16 états)
    hash ^= zobrist_castling[board->castling & 0xF];

    // En passant (colonne)
    if (board->enpassant)
    {
        int file = __builtin_ctzll(board->enpassant) % 8;
        hash ^= zobrist_enpassant[file];
    }

    return hash;
}

// A faire pour optimisation
uint64_t update_hash(uint64_t hash, const Move *move, const Chessboard *board)
{
    return compute_hash(board);
}

TTEntry *tt_probe(uint64_t key, int depth, int alpha, int beta)
{
    TTEntry *entry = &transposition_table[key & TT_MASK];
    if (entry->key == key && entry->depth >= depth)
    {
        if (entry->flag == TT_FLAG_EXACT)
        {
            return entry;
        }
        else if (entry->flag == TT_FLAG_LOWERBOUND && entry->score >= beta)
        {
            return entry;
        }
        else if (entry->flag == TT_FLAG_UPPERBOUND && entry->score <= alpha)
        {
            return entry;
        }
    }
    return NULL;
}

Move tt_get_best_move(uint64_t key)
{
    TTEntry *entry = &transposition_table[key & TT_MASK];
    if (entry->key == key && (entry->best_move.from || entry->best_move.to))
        return entry->best_move;

    Move empty = {0};
    return empty;
}

void tt_store(uint64_t key, int depth, int score, TTFlag flag, Move best_move)
{
    TTEntry *entry = &transposition_table[key & TT_MASK];
    if (entry->key == 0ULL)
        tt_filled++;

    else if (entry->key != key || entry->depth <= depth)
        tt_replaced++;

    if (entry->key != key || entry->depth <= depth)
    {
        entry->key = key;
        entry->depth = depth;
        entry->score = score;
        entry->flag = flag;
        entry->best_move = best_move;
    }
}

void tt_print_stats()
{
    double fill_rate = (double)tt_filled / (double)TT_SIZE * 100.0;
    double replace_rate = 0.0;
    if (tt_filled + tt_replaced > 0)
        replace_rate = (double)tt_replaced / (double)(tt_filled + tt_replaced) * 100.0;

    printf("TT stats:\n");
    printf("  Entries filled:   %llu / %llu (%.2f%%)\n", tt_filled, (unsigned long long)TT_SIZE, fill_rate);
    printf("  Entries replaced: %llu (%.2f%% of writes)\n", tt_replaced, replace_rate);
}
