#include "chessboard.h"
#include "bitboards/masks.h"
#include "../tools/dynamic_list.h"
#include "bitboards/bitboard.h"
#include "chessrules.h"
#include "move/move.h"
#include "hashtables/hash_table.h"

/// @brief renvoie la liste des coups possibles pour une piece
/// @param piecePos la position de la piece
/// @param chessboard l'echequier
/// @return la liste des coups possibles
int get_legal_moves(int piecePos, Chessboard *chessboard, Move piece_moves[250]);

/// @brief Récupère tous les coups légaux possibles pour toutes les pièces du joueur
/// @param chessboard l'échiquier
/// @return Tout les coups légaux possibles
int get_all_legal_moves(Chessboard *chessboard, Move all_moves[250]);

/// @brief Joue un coup sur l'échiquier. Cette fonction met à jour l'echequier
/// @param board l'echequier qui va être mis à jour
/// @param move le coup à jouer
/// @return true si le coup a été joué, false si le coup n'est pas légal
void play_move(Chessboard *board, Move move);

/// @brief Check if the move is legal before playing it
/// @param board
/// @param move
/// @return
bool try_play_move(Chessboard *board, Move *move);

/// @brief Permet de déjouer le coup précédent.
/// ATTENTION: la fonction ne vérifie pas que le coup à déjouer soit valide!!!
/// Il faut que l'utilisateur fasse attention à ce que les  coups soit déjoué dans le bon ordre
/// @param board
/// @param move
void unplay_move(Chessboard *board, Move move);

/// @brief Renvoie true si il y a echec
/// @param board
/// @return
bool is_check(Chessboard *board);

/// @brief Renvoie toute les captures possibles de la position
/// @param chessboard
/// @param all_moves
/// @return
int get_all_captures(Chessboard *chessboard, Move captures[250]);

/// @brief Joue le coup et renvoie le "Game State"
/// Playing = 0;
/// Draw = 1;
/// Checkmate = 2
/// @param board
/// @param move
/// @return
int play_move_check_gameover(Chessboard *board, Move move);
