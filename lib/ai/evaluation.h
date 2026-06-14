#include "chessboard/chessboard.h"
#include <stdlib.h>

#define PAWN_VALUE 100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE 500
#define QUEEN_VALUE 900

#define VALUE_PIECES_START (2 * QUEEN_VALUE + 4 * ROOK_VALUE + 4 * BISHOP_VALUE + 4 * KNIGHT_VALUE)

extern int pawn_table[BOARD_SIZE];
extern int knight_table[BOARD_SIZE];
extern int bishop_table[BOARD_SIZE];
extern int rook_table[BOARD_SIZE];
extern int queen_table[BOARD_SIZE];
extern int king_table[BOARD_SIZE];
extern int king_table_endgame[BOARD_SIZE];



/// @brief Renvoie l'evaluation de la position
/// @param board
/// @return
int evaluate_position(Chessboard *board);

/// @brief Initialise les tables de position valeur utilisés pour l'évaluation des cases
/// @return
int initialize_tables(void);

/// @brief Renvoie la valeur de la piece sur la case 'square'
/// @param board 
/// @param square 
/// @return 
int get_piece_value(Chessboard *board, int square);