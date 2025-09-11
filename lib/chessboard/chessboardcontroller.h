#include "chessboard.h"
#include "bitboards/masks.h"
#include "../tools/dynamic_list.h"
#include "bitboards/bitboard.h"
#include "chessrules.h"
#include "move/move.h"

/// @brief renvoie la liste des coups possibles pour une piece
/// @param piecePos la position de la piece
/// @param chessboard l'echequier
/// @return la liste des coups possibles
void getlegalmoves(int piecePos, Chessboard *chessboard, GenericList *moves);

/// @brief Récupère tous les coups légaux possibles pour toutes les pièces du joueur
/// @param chessboard l'échiquier
/// @return Tout les coups légaux possibles
void getalllegalmoves(Chessboard *chessboard, GenericList *allMoves);

/// @brief Joue un coup sur l'échiquier. Cette fonction met à jour l'echequier
/// @param board l'echequier qui va être mis à jour
/// @param move le coup à jouer
/// @return true si le coup a été joué, false si le coup n'est pas légal
void play_move(Chessboard *board, Move move);

/// @brief Check if the move is legal before playing it
/// @param board
/// @param move
/// @return
bool try_play_move(Chessboard *board, Move move);

/// @brief Permet de déjouer le coup précédent.
/// ATTENTION: la fonction ne vérifie pas que le coup à déjouer soit valide!!!
/// Il faut que l'utilisateur fasse attention à ce que les  coups soit déjoué dans le bon ordre
/// @param board
/// @param move
void unplay_move(Chessboard *board, Move move);
