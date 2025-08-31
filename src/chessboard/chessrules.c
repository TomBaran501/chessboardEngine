#include <stdint.h>
#include "chessrules.h"

const int SHORTCASTLE = 0;
const int LONGCASTLE = 1;

const uint64_t FULLMASK = 0xFFFFFFFFFFFFFFFFL;

const uint64_t pos_rook_castle[2][2][2] = {{{1ULL << 63, 1ULL << 61}, {1ULL << 56, 1ULL << 59}}, {{1ULL << 7, 1ULL << 5}, {1ULL << 0, 1ULL << 3}}};
const int castling_pos[2][2] = {{62, 58}, {6, 2}};
const uint64_t castling_empty_squares[2][2] = {{(1ULL << 61) + (1ULL << 62), (1ULL << 58) + (1ULL << 59) + (1ULL << 57)},
                                               {(1ULL << 6) + (1ULL << 5), (1ULL << 1) + (1ULL << 2) + (1ULL << 3)}};

bool is_piece_pinned(int pos_piece, int pos_king, Chessboard *board, int color)
{
    int r1 = pos_piece / 8;
    int f1 = pos_piece % 8;
    int r2 = pos_king / 8;
    int f2 = pos_king % 8;

    // regarde si le roi et la piece sont alignés
    if ((r1 != r2) && (f1 != f2) && ((r1 - f1) != (r2 - f2)) && ((r1 + f1) == (r2 + f2)))
        return false;

    uint64_t pieces = (board->occupied_black) | (board->occupied_white);
    uint64_t piece = create_1bit_board(pos_piece);
    uint64_t ennemies = color == WHITE ? board->occupied_black : board->occupied_white;
    uint64_t ennemiesRQ = ennemies & (board->rooks | board->queens);
    uint64_t ennemiesBQ = ennemies & (board->bishops | board->queens);

    uint64_t candidates = get_bishop_attacks(pos_king, pieces);
    if (candidates & piece)
    {
        uint64_t potential_attackers = get_bishop_attacks(pos_king, (pieces ^ piece)) & ennemies;
        potential_attackers ^= candidates;
        if (potential_attackers & ennemiesBQ)
            return true;
    }

    candidates = get_rook_attacks(pos_king, pieces);
    if (candidates & piece)
    {
        uint64_t potential_attackers = get_rook_attacks(pos_king, (pieces ^ piece)) & ennemies;
        potential_attackers ^= candidates;
        if (potential_attackers & ennemiesRQ)
            return true;
    }
    return false;
}

uint64_t handle_king_safety(uint64_t piece, int pos_king, Chessboard *board, int color)
{
    if (piece & board->kings)
        return ~get_threatenned_squares(board, pos_king);

    if (is_piece_pinned(get_lsb_index(piece), pos_king, board, color))
        return line_mask(pos_king, get_lsb_index(piece));

    return FULLMASK;
}

uint64_t get_attacks(int piecePos, Chessboard *chessboard, uint64_t pieces)
{
    uint64_t pos = create_1bit_board(piecePos);
    int color = (chessboard->occupied_white & pos) ? WHITE : BLACK;
    uint64_t ennemies = color == WHITE ? chessboard->occupied_black : chessboard->occupied_white;

    if (chessboard->pawns & pos)
        return masks_pawn_captures[color][piecePos] & (ennemies | chessboard->enpassant);
    else if (chessboard->knights & pos)
        return masks_knight_moves[piecePos];
    else if (chessboard->bishops & pos)
        return get_bishop_attacks(piecePos, pieces);
    else if (chessboard->rooks & pos)
        return get_rook_attacks(piecePos, pieces);
    else if (chessboard->queens & pos)
        return get_bishop_attacks(piecePos, pieces) | get_rook_attacks(piecePos, pieces);
    else if (chessboard->kings & pos)
        return masks_king_moves[piecePos];
    else
        return 0;
}

uint64_t getattacks(int piecePos, Chessboard *chessboard)
{
    return get_attacks(piecePos, chessboard, chessboard->occupied_black | chessboard->occupied_white);
}

uint64_t handle_pawn_moves(int pos_piece, Chessboard *board)
{
    uint64_t from_bitboard = create_1bit_board(pos_piece);
    if (!(from_bitboard & board->pawns))
        return 0;

    int color = (board->occupied_white & from_bitboard) ? WHITE : BLACK;
    int direction = (color == WHITE) ? -1 : 1;

    if (masks_pawn_moves[color][pos_piece] & (board->occupied_black | board->occupied_white))
        return create_1bit_board(pos_piece + direction * 8) & (board->occupied_black | board->occupied_white) ? 0 : create_1bit_board(pos_piece + direction * 8);

    else
        return masks_pawn_moves[color][pos_piece];
}

// Attention!!! rajouter la vérification des échecs sur les cases ou passent le roi
uint64_t handle_roque_moves(int pos_piece, Chessboard *board)
{
    uint64_t from_bitboard = create_1bit_board(pos_piece);
    if (!(from_bitboard & board->kings))
        return 0;

    int color = (board->occupied_white & from_bitboard) ? WHITE : BLACK;
    uint64_t mask_roque_moves = 0;
    uint64_t occupied_squares = board->occupied_black | board->occupied_white;

    if (((castling_empty_squares[color][SHORTCASTLE] & occupied_squares) == 0) && (board->castling & create_1bit_board(castling_pos[color][SHORTCASTLE])))
        mask_roque_moves |= create_1bit_board(castling_pos[color][SHORTCASTLE]);

    if (((castling_empty_squares[color][LONGCASTLE] & occupied_squares) == 0) && (board->castling & create_1bit_board(castling_pos[color][LONGCASTLE])))
        mask_roque_moves |= create_1bit_board(castling_pos[color][LONGCASTLE]);

    return mask_roque_moves;
}

uint64_t pieces_attacking_square(int square, Chessboard *board, int color)
{
    uint64_t ennemies = color == WHITE ? board->occupied_black : board->occupied_white;
    uint64_t pieces = (board->occupied_black) | (board->occupied_white);
    uint64_t attacking_pieces = 0;

    attacking_pieces |= get_bishop_attacks(square, pieces) & ((board->queens | board->bishops) & ennemies);
    attacking_pieces |= get_rook_attacks(square, pieces) & ((board->queens | board->rooks) & ennemies);
    attacking_pieces |= masks_pawn_captures[color][square] & (board->pawns & ennemies);
    attacking_pieces |= masks_knight_moves[square] & (board->knights & ennemies);

    return attacking_pieces;
}

uint64_t get_threatenned_squares(Chessboard *board, int pos_king)
{
    uint64_t board_without_king = (board->occupied_black | board->occupied_white) ^ create_1bit_board(pos_king);
    uint64_t ennemies = board->white_to_play ? board->occupied_black : board->occupied_white;
    uint64_t threatenned_squares = 0;

    while (ennemies)
    {
        int square = get_lsb_index(ennemies);
        ennemies = pop_bit(ennemies);

        threatenned_squares |= get_attacks(square, board, board_without_king);
    }
    return threatenned_squares;
}

uint64_t get_blocks_check(int pos_king, uint64_t attacker, Chessboard *board)
{
    uint64_t occ = board->occupied_black | board->occupied_white;
    int pos_att = get_lsb_index(attacker);
    if (pos_king / 8 != pos_att / 8 && pos_king % 8 != pos_att % 8)
        return (get_bishop_attacks(pos_king, occ) & get_bishop_attacks(pos_att, occ)) | attacker;
    else
        return (get_rook_attacks(pos_king, occ) & get_rook_attacks(pos_att, occ)) | attacker;
}

uint64_t handle_checks(int pos_king, Chessboard *board, int color)
{
    uint64_t attackers = pieces_attacking_square(pos_king, board, color);
    int nbattackers = count_bits(attackers);

    if (nbattackers == 1)
        return get_blocks_check(pos_king, attackers, board);
    if (nbattackers > 1)
        return 0;
    return FULLMASK;
}
