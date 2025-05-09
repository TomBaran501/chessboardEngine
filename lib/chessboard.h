#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


// Définition de la structure Chessboard
typedef struct
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
    
} Chessboard;

// Initialisation d'un échiquier avec la position de départ
void init_chessboard(Chessboard *board);

/// @brief This function prints the chessboard
/// @param board the chessboard to print
void print_chessboard(const Chessboard *board);


#endif