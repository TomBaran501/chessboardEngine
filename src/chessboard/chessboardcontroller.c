#include "chessboardcontroller.h"

#include <string.h>

void handle_pawn_flags(Move *move, Chessboard *chessboard)
{
    if (chessboard->pawns & create_1bit_board(move->from))
    {
        if (abs(move->to - move->from) == 16)
            set_pawn_advanced2(move, true);

        else if (create_1bit_board(move->to) == chessboard->enpassant)
            set_enpassant(move, true);
    }
    move->en_passant = chessboard->enpassant;
}

void update_roque_bitboard(Chessboard *board, Move *move, int color)
{
    int pos_piece = move->from;
    uint64_t piece = create_1bit_board(pos_piece);
    if (piece & board->kings)
    {
        board->castling &= ~((1ULL << castling_pos[color][SHORTCASTLE]) + (1ULL << castling_pos[color][LONGCASTLE]));
        move->roque_broken = LONGCASTLEBROKEN + SHORTCASTLEBROKEN;
    }

    else if (piece & board->rooks)
    {
        if (pos_piece == get_lsb_index(pos_rook_castle[color][SHORTCASTLE][0]))
        {
            board->castling &= ~(1ULL << castling_pos[color][SHORTCASTLE]);
            move->roque_broken = SHORTCASTLEBROKEN;
        }

        else if (pos_piece == get_lsb_index(pos_rook_castle[color][LONGCASTLE][0]))
        {
            board->castling &= ~(1ULL << castling_pos[color][LONGCASTLE]);
            move->roque_broken = LONGCASTLEBROKEN;
        }
    }
}

void add_roque_rights(Chessboard *board, int color, uint16_t roque_broken_flag)
{
    if (roque_broken_flag & LONGCASTLEBROKEN)
        board->castling |= (1ULL << castling_pos[color][LONGCASTLE]);
    if (roque_broken_flag & SHORTCASTLEBROKEN)
        board->castling |= (1ULL << castling_pos[color][SHORTCASTLE]);
}

void handle_roque_flags(Move *move, Chessboard *chessboard)
{
    if (chessboard->kings & create_1bit_board(move->from))
    {
        if (move->to - move->from == 2)
            set_short_castle(move, true);

        if (move->to - move->from == -2)
            set_long_castle(move, true);
    }
}

void handle_piece_taken_flag(Move *move, Chessboard *chessboard)
{
    if (get_enpassant(*move) || create_1bit_board(move->to) & chessboard->pawns)
        move->piece_taken = PAWN;
    else if (create_1bit_board(move->to) & chessboard->knights)
        move->piece_taken = KNIGHT;
    else if (create_1bit_board(move->to) & chessboard->queens)
        move->piece_taken = QUEEN;
    else if (create_1bit_board(move->to) & chessboard->bishops)
        move->piece_taken = BISHOP;
    else if (create_1bit_board(move->to) & chessboard->rooks)
        move->piece_taken = ROOK;
    else
        move->piece_taken = NONE;
}

void handle_promotion(GenericList *moves, Chessboard *board)
{
    if (moves->size <= 3 && moves->size > 0)
    {
        int original_size = moves->size;
        for (int i = 0; i < original_size; i++)
        {
            Move *move = (Move *)list_get(moves, i);
            if ((board->pawns & (1ULL << move->from)) && (move->to / 8 == 0 || move->to / 8 == 7))
            {
                Move promo;
                move->promotion_flag = PROMOTION_N;

                promo = *move;
                promo.promotion_flag = PROMOTION_B;
                list_add(moves, &promo);

                promo = *move;
                promo.promotion_flag = PROMOTION_R;
                list_add(moves, &promo);

                promo = *move;
                promo.promotion_flag = PROMOTION_Q;
                list_add(moves, &promo);
            }
        }
    }
}

uint64_t get_king_pos(Chessboard *board, int color)
{
    uint64_t allies = color == WHITE ? board->occupied_white : board->occupied_black;
    return get_lsb_index(allies & board->kings);
}

void add_flags(Move *move, Chessboard *chessboard)
{
    handle_pawn_flags(move, chessboard);
    handle_roque_flags(move, chessboard);
    handle_piece_taken_flag(move, chessboard);
}

GenericList *getlegalmoves(int piecePos, Chessboard *chessboard)
{
    GenericList *moves = malloc(sizeof(GenericList));
    list_init(moves, sizeof(Move));
    uint64_t playerPieces = chessboard->white_to_play ? chessboard->occupied_white : chessboard->occupied_black;

    if ((create_1bit_board(piecePos) & playerPieces) == 0)
        return moves;

    int color = chessboard->white_to_play ? WHITE : BLACK;
    int king_square = get_lsb_index(playerPieces & chessboard->kings);

    uint64_t attacks = get_attacks(piecePos, chessboard);
    attacks |= handle_pawn_moves(piecePos, chessboard);
    attacks = handle_king_safety(create_1bit_board(piecePos), king_square, chessboard, color, attacks);

    attacks -= playerPieces & attacks;

    int nbmoves = count_bits(attacks);

    for (int m = 0; m < nbmoves; m++)
    {
        Move move;
        initialise_move(&move, piecePos, get_lsb_index(attacks));
        add_flags(&move, chessboard);
        list_add(moves, &move);

        attacks = pop_bit(attacks);
    }
    handle_promotion(moves, chessboard);
    return moves;
}

GenericList *getalllegalmoves(Chessboard *chessboard)
{
    GenericList *allMoves = malloc(sizeof(GenericList));
    list_init(allMoves, sizeof(Move));
    uint64_t playerPieces = chessboard->white_to_play ? chessboard->occupied_white : chessboard->occupied_black;

    while (playerPieces)
    {
        int piecePos = get_lsb_index(playerPieces);
        GenericList *pieceMoves = getlegalmoves(piecePos, chessboard);

        // Ajoute les mouvements de cette pièce à la liste globale
        for (int i = 0; i < pieceMoves->size; i++)
        {
            list_add(allMoves, &((Move *)pieceMoves->data)[i]);
        }

        // Libère la mémoire de la liste temporaire
        list_free(pieceMoves);
        free(pieceMoves);
        playerPieces = pop_bit(playerPieces); // Supprime la pièce traitée
    }

    return allMoves;
}

void update_bitboards_movement(uint64_t from_bitboard, Chessboard *board, uint64_t to_bitboard)
{
    if (from_bitboard & board->rooks)
    {
        board->rooks -= from_bitboard;
        board->rooks += to_bitboard;
    }
    else if (from_bitboard & board->pawns)
    {
        board->pawns -= from_bitboard;
        board->pawns += to_bitboard;
    }
    else if (from_bitboard & board->knights)
    {
        board->knights -= from_bitboard;
        board->knights += to_bitboard;
    }
    else if (from_bitboard & board->bishops)
    {
        board->bishops -= from_bitboard;
        board->bishops += to_bitboard;
    }
    else if (from_bitboard & board->queens)
    {
        board->queens -= from_bitboard;
        board->queens += to_bitboard;
    }
    else if (from_bitboard & board->kings)
    {
        board->kings -= from_bitboard;
        board->kings += to_bitboard;
    }
    if (board->occupied_white & from_bitboard)
    {
        board->occupied_white -= from_bitboard;
        board->occupied_white += to_bitboard;
    }
    else if (board->occupied_black & from_bitboard)
    {
        board->occupied_black -= from_bitboard;
        board->occupied_black += to_bitboard;
    }
}

void update_bitboard_delete_piece(Chessboard *board, uint64_t to_bitboard)
{
    if (to_bitboard & board->rooks)
        board->rooks -= to_bitboard;

    else if (to_bitboard & board->pawns)
        board->pawns -= to_bitboard;

    else if (to_bitboard & board->knights)
        board->knights -= to_bitboard;

    else if (to_bitboard & board->bishops)
        board->bishops -= to_bitboard;

    else if (to_bitboard & board->queens)
        board->queens -= to_bitboard;

    else if (to_bitboard & board->kings)
        board->kings -= to_bitboard;

    if (board->occupied_white & to_bitboard)
        board->occupied_white -= to_bitboard;

    else if (board->occupied_black & to_bitboard)
        board->occupied_black -= to_bitboard;
}

void update_bitboard_create_piece(Chessboard *board, uint64_t to_bitboard, uint16_t piece_flag, int color)
{
    if (piece_flag == ROOK)
        board->rooks |= to_bitboard;

    else if (piece_flag == PAWN)
        board->pawns |= to_bitboard;

    else if (piece_flag == KNIGHT)
        board->knights |= to_bitboard;

    else if (piece_flag == BISHOP)
        board->bishops |= to_bitboard;

    else if (piece_flag == QUEEN)
        board->queens |= to_bitboard;

    else if (piece_flag == KING)
        board->kings |= to_bitboard;

    if (color == WHITE)
        board->occupied_white |= to_bitboard;

    else
        board->occupied_black |= to_bitboard;
}

void update_bitboard_promotion(int promotion_flag, Chessboard *board, uint64_t to_bitboard)
{
    update_bitboard_delete_piece(board, to_bitboard);

    if (promotion_flag == PROMOTION_B)
        board->bishops |= to_bitboard;
    else if (promotion_flag == PROMOTION_N)
        board->knights |= to_bitboard;
    else if (promotion_flag == PROMOTION_R)
        board->rooks |= to_bitboard;
    else if (promotion_flag == PROMOTION_Q)
        board->queens |= to_bitboard;

    if (board->white_to_play)
        board->occupied_white |= to_bitboard;
    else
        board->occupied_black |= to_bitboard;
}

bool is_legal_move(Chessboard *board, Move *move)
{
    GenericList *legalMoves = getalllegalmoves(board);

    for (int i = 0; i < legalMoves->size; i++)
    {
        if (((Move *)legalMoves->data)[i].from == move->from && ((Move *)legalMoves->data)[i].to == move->to)
        {
            list_free(legalMoves);
            free(legalMoves);
            return true;
        }
    }
    list_free(legalMoves);
    free(legalMoves);
    return false;
}

bool try_play_move(Chessboard *board, Move move)
{
    if (!is_legal_move(board, &move))
        return false;

    add_flags(&move, board);

    play_move(board, move);
    return true;
}

// ATTENTION: le flag roque broken prend une valeur significative dans play_move
//           et pas dans get_legal_move comme les autres flags
void play_move(Chessboard *board, Move move)
{
    uint64_t from_bitboard = create_1bit_board(move.from);
    uint64_t to_bitboard = create_1bit_board(move.to);

    int color = board->white_to_play ? WHITE : BLACK;

    if (get_enpassant(move))
    {
        uint64_t pos_ennemie = board->white_to_play ? move.to + 8 : move.to - 8;
        update_bitboard_delete_piece(board, create_1bit_board(pos_ennemie));
    }

    if (get_long_castle(move))
        update_bitboards_movement(pos_rook_castle[color][LONGCASTLE][0], board, pos_rook_castle[color][LONGCASTLE][1]);

    if (get_short_castle(move))
        update_bitboards_movement(pos_rook_castle[color][SHORTCASTLE][0], board, pos_rook_castle[color][SHORTCASTLE][1]);

    if (board->castling != 0)
        update_roque_bitboard(board, &move, color);

    update_bitboard_delete_piece(board, to_bitboard);
    update_bitboards_movement(from_bitboard, board, to_bitboard);

    if (move.promotion_flag != 0)
        update_bitboard_promotion(move.promotion_flag, board, to_bitboard);

    board->enpassant = 0;

    if (get_pawn_advanced2(move))
    {
        int dir = board->white_to_play ? 1 : -1;
        board->enpassant = create_1bit_board(dir * 8 + move.to);
    }

    board->white_to_play = !board->white_to_play;
}

void unplay_move(Chessboard *board, Move move)
{
    uint64_t from_bitboard = create_1bit_board(move.from);
    uint64_t to_bitboard = create_1bit_board(move.to);

    int color = board->white_to_play ? WHITE : BLACK;
    int ennemi_color = color == WHITE ? BLACK : WHITE;

    if (get_enpassant(move))
    {
        uint64_t pos_ennemie = 1ULL << (ennemi_color == WHITE ? move.to + 8 : move.to - 8);
        update_bitboard_create_piece(board, pos_ennemie, PAWN, color);
    }

    if (get_long_castle(move))
        update_bitboards_movement(pos_rook_castle[ennemi_color][LONGCASTLE][1], board, pos_rook_castle[ennemi_color][LONGCASTLE][0]);

    if (get_short_castle(move))
        update_bitboards_movement(pos_rook_castle[ennemi_color][SHORTCASTLE][1], board, pos_rook_castle[ennemi_color][SHORTCASTLE][0]);

    if (move.roque_broken != 0)
        add_roque_rights(board, ennemi_color, move.roque_broken);

    update_bitboards_movement(to_bitboard, board, from_bitboard);

    if (move.piece_taken != NONE)
        update_bitboard_create_piece(board, to_bitboard, move.piece_taken, color);

    if (move.promotion_flag != 0)
    {
        update_bitboard_delete_piece(board, from_bitboard);
        update_bitboard_create_piece(board, from_bitboard, PAWN, ennemi_color);
    }
    board->enpassant = move.en_passant;
    board->white_to_play = !board->white_to_play;
}
