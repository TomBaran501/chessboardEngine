#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>
#include <stdbool.h>

// Structure représentant un mouvement dans le jeu d'échecs
typedef struct
{
    uint16_t from : 6; // 6 bits pour la case de départ (0-63)
    uint16_t to : 6;   // 6 bits pour la case d'arrivée (0-63)
    uint16_t flag : 4; // 4 bits pour les types spéciaux de mouvements
} Move;

/// @brief permet d'initialiser un coup avec toutes les valeurs à 0
/// @param move
void initialise_empty_move(Move *move);

/// @brief permet d'initialiser un coup avec une valeur pour la position de départ et d'arriver de la piece qui bouge.
/// @param move le coup initialisé
/// @param from case de départ
/// @param to case d'arrivée
void initialise_move(Move *move, int from, int to);

/// @brief Prints the parameter move
/// @param move
void print_move(Move *move);

bool get_long_castle(Move move);
void set_long_castle(Move *move, bool value);

bool get_short_castle(Move move);
void set_short_castle(Move *move, bool value);

bool get_pawn_advanced2(Move move);
void set_pawn_advanced2(Move *move, bool value);

bool get_promotion(Move move);
void set_promotion(Move *move, bool value);

#endif
