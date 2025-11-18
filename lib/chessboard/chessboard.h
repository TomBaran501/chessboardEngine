#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "bitboard.h"
#include "masks.h"

typedef struct RepetitionTable RepetitionTable;

// Définition de la structure Chessboard
typedef struct Chessboard
{
    uint64_t pawns;
    uint64_t knights;
    uint64_t bishops;
    uint64_t rooks;
    uint64_t queens;
    uint64_t kings;

    uint64_t occupied_white;
    uint64_t occupied_black;

    uint64_t enpassant;
    uint64_t castling;

    int white_to_play;

    RepetitionTable *hashtable;

} Chessboard;

enum
{
    PLAYING = 0,
    DRAW = 1,
    WIN = 2,
}; // Game state

// Initialisation d'un échiquier avec la position de départ
void init_chessboard(Chessboard *board);

/// @brief This function prints the chessboard
/// @param board the chessboard to print
void print_chessboard(const Chessboard *board);

/// @brief This function generate the fen of a position
/// @param board the chessboard we want the fen from
char *return_fen_code(Chessboard *board, char *fen);

/// @brief This function initialise the chessboard from its code fen
/// @param board the chessboard we want to initialise
/// @param fen the code of the position
void init_chessboard_from_fen(Chessboard *board, const char *fen);

void copy_chessboard(const Chessboard *src, Chessboard *dst);

bool compare_chessboard(const Chessboard *a, const Chessboard *b);

void print_chessboard_diff(const Chessboard *a, const Chessboard *b);

#endif