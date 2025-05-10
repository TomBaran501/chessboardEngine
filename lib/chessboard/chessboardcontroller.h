#include "chessboard.h"
#include "bitboards/masks.h"
#include "move/movelist.h"
#include "bitboards/bitboard.h"
#include "chessrules.h"

/// @brief renvoie la liste des coups possibles pour une piece
/// @param piecePos la position de la piece
/// @param chessboard l'echequier
/// @return la liste des coups possibles
MoveList *getlegalmoves(int piecePos, Chessboard *chessboard);

/// @brief Récupère tous les coups légaux possibles pour toutes les pièces du joueur
/// @param chessboard l'échiquier
/// @return Tout les coups légaux possibles
MoveList *getalllegalmoves(Chessboard *chessboard);

/// @brief Joue un coup sur l'échiquier. Cette fonction met à jour l'echequier
/// @param board l'echequier qui va être mis à jour
/// @param move le coup à jouer
/// @return true si le coup a été joué, false si le coup n'est pas légal
bool play_move(Chessboard *board, Move move);