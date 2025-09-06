#include "chessboard.h"
#include "bitboards/masks.h"

extern const int SHORTCASTLE;
extern const int LONGCASTLE;
extern const uint64_t pos_rook_castle[2][2][2];
extern const int castling_pos[2][2];
extern const uint64_t castling_empty_squares[2][2];

/// @brief Renvoie les coups possibles de bases pour une piece
/// @param piecePos
/// @param chessboard
/// @return
uint64_t getattacks(int piecePos, Chessboard *chessboard);

/// @brief Renvoie les coups possibles (hors captures) pour un pion
/// @param pos_piece le numéro de la case du pion
/// @param board l'echequier
/// @return les nouveaux coups possibles pour le pion
uint64_t handle_pawn_moves(int pos_piece, Chessboard *board);

/// @brief Renvoie les roques possible pour un roi
/// @param pos_piece
/// @param board
/// @return les roques possibles
uint64_t handle_roque_moves(int pos_piece, Chessboard *board, uint64_t threatenned_squares);

/// @brief Renvoie les pieces ennemies attaquant la case square
/// @param square
/// @param board
/// @param color la couleur alliée
/// @return
uint64_t pieces_attacking_square(int square, Chessboard *board, int color);

/// @brief Renvoie toutes les cases menacées par les ennemis
/// @param board
/// @param pos_king
/// @return
uint64_t get_threatenned_squares(Chessboard *board, int pos_king);

/// @brief Renvoie les cases authorisées pour bloquer l'echec (si il y a echec)
/// @param pos_king
/// @param board
/// @param color
/// @return
uint64_t handle_checks(int pos_king, Chessboard *board, int color);

/// @brief Renvoie un mask des coups qui ne mettent pas en danger le roi pour la piece
/// @param piece
/// @param pos_king
/// @param board
/// @param color
/// @return
uint64_t handle_king_safety(uint64_t piece, int pos_king, Chessboard *board, int color, uint64_t attacks);

