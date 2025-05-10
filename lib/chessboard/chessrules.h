#include "chessboard.h"
#include "bitboards/masks.h"


/// @brief Renvoie les coups possibles (hors captures) pour un pion
/// @param pos_piece le numéro de la case du pion
/// @param board l'echequier
/// @return les nouveaux coups possibles pour le pion
uint64_t handle_pawn_moves(int pos_piece, Chessboard *board);