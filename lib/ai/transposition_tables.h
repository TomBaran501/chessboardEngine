#ifndef TRANSPOSITION_TABLES_H
#define TRANSPOSITION_TABLES_H

#include <stdint.h>
#include "chessboard/move/move.h"
#include "chessboard/chessboard.h"

// Taille de la table : puissance de 2
#define TT_SIZE (1 << 21) // ≈ 1 million d’entrées
#define TT_MASK (TT_SIZE - 1)

// Flags pour indiquer le type de valeur stockée
typedef enum
{
    TT_FLAG_EXACT,      // valeur exacte
    TT_FLAG_LOWERBOUND, // coupure beta (fail-high)
    TT_FLAG_UPPERBOUND  // coupure alpha (fail-low)
} TTFlag;

// Une entrée dans la transposition table
typedef struct
{
    uint64_t key;   // Zobrist hash
    int depth;      // profondeur de recherche de l’entrée
    int score;      // valeur évaluée
    TTFlag flag;    // type de valeur
    Move best_move;
} TTEntry;

// Initialise le Zobrist hashing (à appeler au lancement du programme)
void init_zobrist();

// Calcule le hash complet d’un échiquier
uint64_t compute_hash(const Chessboard *board);

// Récupère une entrée dans la table (NULL si vide ou non pertinente)
TTEntry *tt_probe(uint64_t key, int depth, int alpha, int beta);

// Stocke une nouvelle entrée dans la table
void tt_store(uint64_t key, int depth, int score, TTFlag flag, Move best_move);

void tt_print_stats();

Move tt_get_best_move(uint64_t key);
#endif
