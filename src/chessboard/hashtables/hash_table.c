#include "hash_table.h"
#include "../chessboard.h"

ZobristKey zobrist_pieces[12][64];
ZobristKey zobrist_castling[16];
ZobristKey zobrist_en_passant[64];
ZobristKey zobrist_side_to_move;

static bool zobrist_initialized = false;

uint64_t rand64(void)
{
    uint64_t r1 = (uint64_t)rand();
    uint64_t r2 = (uint64_t)rand();
    uint64_t r3 = (uint64_t)rand();
    uint64_t r4 = (uint64_t)rand();
    return (r1 << 48) | (r2 << 32) | (r3 << 16) | r4;
}

void init_zobrist(void)
{
    if (zobrist_initialized)
        return;

    srand(123456789);

    for (int piece = 0; piece < 12; piece++)
    {
        for (int square = 0; square < 64; square++)
            zobrist_pieces[piece][square] = rand64();
    }

    for (int i = 0; i < 16; i++)
        zobrist_castling[i] = rand64();

    for (int i = 0; i < 64; i++)
        zobrist_en_passant[i] = rand64();

    zobrist_side_to_move = rand64();
    zobrist_initialized = true;
}

// Fonction helper pour obtenir l'index de pièce
// 0-5: pièces blanches (P,N,B,R,Q,K), 6-11: pièces noires
static int get_piece_index(const Chessboard *board, int square)
{
    uint64_t mask = 1ULL << square;
    bool is_white = (board->occupied_white & mask) != 0;

    if (board->pawns & mask)
        return is_white ? 0 : 6;
    if (board->knights & mask)
        return is_white ? 1 : 7;
    if (board->bishops & mask)
        return is_white ? 2 : 8;
    if (board->rooks & mask)
        return is_white ? 3 : 9;
    if (board->queens & mask)
        return is_white ? 4 : 10;
    if (board->kings & mask)
        return is_white ? 5 : 11;

    return -1; // Pas de pièce
}

// Calcul du hash complet d'une position
ZobristKey compute_hash(const Chessboard *board)
{
    ZobristKey hash = 0;

    uint64_t occupied = board->occupied_white | board->occupied_black;
    while (occupied)
    {
        int square = __builtin_ctzll(occupied);
        int piece_index = get_piece_index(board, square);
        if (piece_index >= 0)
        {
            hash ^= zobrist_pieces[piece_index][square];
        }
        occupied &= occupied - 1;
    }

    uint8_t castling_rights = board->castling & 0xF;
    hash ^= zobrist_castling[castling_rights];

    if (board->enpassant)
    {
        int ep_square = __builtin_ctzll(board->enpassant);
        hash ^= zobrist_en_passant[ep_square];
    }

    // XOR trait
    if (!board->white_to_play)
        hash ^= zobrist_side_to_move;

    return hash;
}

// Initialisation de la table de répétition
void init_repetition_table(RepetitionTable *table)
{
    memset(table, 0, sizeof(RepetitionTable));
}

// Ajouter une position à la table
void add_position(RepetitionTable *table, ZobristKey hash)
{
    int index = hash % HASH_TABLE_SIZE;

    // Linear probing pour gérer les collisions
    while (table->entries[index].count > 0 &&
           table->entries[index].key != hash)
    {
        index = (index + 1) % HASH_TABLE_SIZE;
    }

    // Si c'est une nouvelle entrée, définir la clé
    if (table->entries[index].count == 0)
        table->entries[index].key = hash;

    table->entries[index].count++;

    // Ajouter à l'historique
    if (table->history_count < 512)
        table->history[table->history_count++] = hash;
}

// Retirer une position de la table
void remove_position(RepetitionTable *table, ZobristKey hash)
{
    int index = hash % HASH_TABLE_SIZE;

    // Linear probing pour trouver l'entrée
    while (table->entries[index].count > 0 &&
           table->entries[index].key != hash)
    {
        index = (index + 1) % HASH_TABLE_SIZE;
    }

    if (table->entries[index].count > 0)
    {
        table->entries[index].count--;
    }
}

// Retirer la dernière position de l'historique
void pop_position(RepetitionTable *table)
{
    if (table->history_count > 0)
    {
        table->history_count--;
        remove_position(table, table->history[table->history_count]);
    }
}

// Vérifier s'il y a répétition triple
bool is_threefold_repetition(const RepetitionTable *table, ZobristKey hash)
{
    int index = hash % HASH_TABLE_SIZE;

    // Linear probing pour trouver l'entrée
    while (table->entries[index].count > 0 &&
           table->entries[index].key != hash)
    {
        index = (index + 1) % HASH_TABLE_SIZE;
    }

    return table->entries[index].count >= 3;
}

// Vider la table de répétition
void clear_repetition_table(RepetitionTable *table)
{
    memset(table, 0, sizeof(RepetitionTable));
}