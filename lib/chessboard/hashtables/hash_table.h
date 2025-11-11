#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "../chessboard.h"

typedef uint64_t ZobristKey;

#define HASH_TABLE_SIZE 16384

// Structure pour une entrée de la table de répétition
typedef struct {
    ZobristKey key;
    int count;
} HashEntry;

// Table de répétition
typedef struct {
    HashEntry entries[HASH_TABLE_SIZE];
    ZobristKey history[512];
    int history_count;
} RepetitionTable;

// Tables Zobrist globales
extern ZobristKey zobrist_pieces[12][64];  // 12 types de pièces × 64 cases
extern ZobristKey zobrist_castling[16];     // 16 combinaisons de droits de roque
extern ZobristKey zobrist_en_passant[64];   // En passant par case
extern ZobristKey zobrist_side_to_move;     // Trait aux blancs/noirs

// Fonctions d'initialisation
void init_zobrist(void);
uint64_t rand64(void);

// Calcul de hash
ZobristKey compute_hash(const Chessboard *board);

// Gestion de la table de répétition
void init_repetition_table(RepetitionTable *table);
void add_position(RepetitionTable *table, ZobristKey hash);
void remove_position(RepetitionTable *table, ZobristKey hash);
void pop_position(RepetitionTable *table);
bool is_threefold_repetition(const RepetitionTable *table, ZobristKey hash);
void clear_repetition_table(RepetitionTable *table);

#endif // HASH_H