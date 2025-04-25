#include "chessboardcontroller.h"

#include <string.h>

uint64_t getattacks(int piecePos, Chessboard *chessboard)
{
    uint64_t pos = create_1bit_board(piecePos);
    int color = (chessboard->occupied_white & pos) ? WHITE : BLACK;
    uint64_t ennemies = color == WHITE ? chessboard->occupied_black : chessboard->occupied_white;

    if (chessboard->pawns & pos)
        return masks_pawn_captures[color][piecePos] & ennemies;
    else if (chessboard->knights & pos)
        return masks_knight_moves[piecePos];
    else if (chessboard->bishops & pos)
        return get_bishop_attacks(piecePos, (chessboard->occupied_black) || (chessboard->occupied_white));
    else if (chessboard->rooks & pos)
        return get_rook_attacks(piecePos, (chessboard->occupied_black) || (chessboard->occupied_white));
    else if (chessboard->queens & pos)
        return get_bishop_attacks(piecePos, (chessboard->occupied_black) || (chessboard->occupied_white)) || get_rook_attacks(piecePos, (chessboard->occupied_black) || (chessboard->occupied_white));
    else if (chessboard->kings & pos)
        return masks_king_moves[piecePos];
    else
        return 0;
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
        {
            addMove(allMoves, pieceMoves->moves[i]);
        }

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
    if (board->white_to_play)
    {
        board->occupied_white -= from_bitboard;
        board->occupied_white += to_bitboard;
    }
    else
    {
        board->occupied_black -= from_bitboard;
        board->occupied_black += to_bitboard;
    }
}

void update_bitboards_captures(Chessboard *board, uint64_t to_bitboard)
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
    printf("play_move: %d -> %d\n", move.from, move.to);

    if (ismoveinlist(legalMoves, move) == false)
    {
        return false; // Le mouvement n'est pas valide
    }

    uint64_t from_bitboard = create_1bit_board(move.from);
    uint64_t to_bitboard = create_1bit_board(move.to);

    update_bitboards_captures(board, to_bitboard);
    update_bitboards_movement(from_bitboard, board, to_bitboard);

    board->white_to_play = !board->white_to_play;

    return true; // Ajout d'un retour pour indiquer que le mouvement a été joué
}
