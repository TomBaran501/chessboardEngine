#include "chessboardcontroller.h"

#include <string.h>

uint64_t getattacks(int piecePos, Chessboard *chessboard)
{
    uint64_t pos = create_1bit_board(piecePos);
    int color = (chessboard->occupied_white & pos) ? WHITE : BLACK;
    uint64_t ennemies = color == WHITE ? chessboard->occupied_black : chessboard->occupied_white;
    uint64_t pieces = (chessboard->occupied_black) | (chessboard->occupied_white);

    if (chessboard->pawns & pos)
        return masks_pawn_captures[color][piecePos] & (ennemies | chessboard->enpassant);
    else if (chessboard->knights & pos)
        return masks_knight_moves[piecePos];
    else if (chessboard->bishops & pos)
        return get_bishop_attacks(piecePos, pieces & bishop_masks[piecePos]);
    else if (chessboard->rooks & pos)
        return get_rook_attacks(piecePos, pieces & rook_masks[piecePos]);
    else if (chessboard->queens & pos)
        return get_bishop_attacks(piecePos, pieces & bishop_masks[piecePos]) | get_rook_attacks(piecePos, pieces & rook_masks[piecePos]);
    else if (chessboard->kings & pos)
        return masks_king_moves[piecePos];
    else
        return 0;
}

void handle_pawn_flags(Move *move, Chessboard *chessboard)
{
    if (chessboard->pawns & create_1bit_board(move->from))
    {
        if (abs(move->to - move->from) == 16)
            set_pawn_advanced2(move, true);

        else if (create_1bit_board(move->to) == chessboard->enpassant)
            set_enpassant(move, true);
    }
}

void add_flags(Move *move, Chessboard *chessboard)
{
    handle_pawn_flags(move, chessboard);
}

MoveList *getlegalmoves(int piecePos, Chessboard *chessboard)
{
    MoveList *moves = createMoveList(10);
    uint64_t attacks = getattacks(piecePos, chessboard);
    uint64_t playerPieces = chessboard->white_to_play ? chessboard->occupied_white : chessboard->occupied_black;

    attacks |= handle_pawn_moves(piecePos, chessboard);
    attacks -= playerPieces & attacks;

    int nbmoves = count_bits(attacks);

    for (int m = 0; m < nbmoves; m++)
    {
        Move move;
        initialise_move(&move, piecePos, get_lsb_index(attacks));
        add_flags(&move, chessboard);
        addMove(moves, move);
        attacks = pop_bit(attacks);
    }
    return moves;
}

MoveList *getalllegalmoves(Chessboard *chessboard)
{
    MoveList *allMoves = createMoveList(50); // Taille initiale arbitraire
    uint64_t playerPieces = chessboard->white_to_play ? chessboard->occupied_white : chessboard->occupied_black;

    while (playerPieces)
    {
        int piecePos = get_lsb_index(playerPieces);
        MoveList *pieceMoves = getlegalmoves(piecePos, chessboard);

        // Ajoute les mouvements de cette pièce à la liste globale
        for (int i = 0; i < pieceMoves->count; i++)
            addMove(allMoves, pieceMoves->moves[i]);

        freeMoveList(pieceMoves);             // Libère la mémoire de la liste temporaire
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

bool play_move(Chessboard *board, Move move)
{
    MoveList *legalMoves = getalllegalmoves(board);

    if (ismoveinlist(legalMoves, move) == false)
        return false; // Le mouvement n'est pas valide

    add_flags(&move, board);

    // Mise a jour des bitboards
    uint64_t from_bitboard = create_1bit_board(move.from);
    uint64_t to_bitboard = create_1bit_board(move.to);

    if (get_enpassant(move))
    {
        uint64_t pos_ennemie = board->white_to_play ? move.to + 8 : move.to - 8;
        update_bitboard_delete_piece(board, create_1bit_board(pos_ennemie));
    }

    update_bitboard_delete_piece(board, to_bitboard);
    update_bitboards_movement(from_bitboard, board, to_bitboard);

    // Mise a jour du plateau
    board->enpassant = 0;

    if (get_pawn_advanced2(move))
    {
        int dir = board->white_to_play ? 1 : -1;
        board->enpassant = create_1bit_board(dir * 8 + move.to);
    }

    board->white_to_play = !board->white_to_play;
    return true; // Ajout d'un retour pour indiquer que le mouvement a été joué
}
