#include "evaluation.h"

int pawn_table[BOARD_SIZE];
int knight_table[BOARD_SIZE];
int bishop_table[BOARD_SIZE];
int rook_table[BOARD_SIZE];
int queen_table[BOARD_SIZE];
int king_table[BOARD_SIZE];

int count_material(Chessboard *board)
{
    int score = 0;

    // Matériel blanc
    score += count_bits(board->pawns & board->occupied_white) * PAWN_VALUE;
    score += count_bits(board->knights & board->occupied_white) * KNIGHT_VALUE;
    score += count_bits(board->bishops & board->occupied_white) * BISHOP_VALUE;
    score += count_bits(board->rooks & board->occupied_white) * ROOK_VALUE;
    score += count_bits(board->queens & board->occupied_white) * QUEEN_VALUE;

    // Matériel noir (on soustrait)
    score -= count_bits(board->pawns & board->occupied_black) * PAWN_VALUE;
    score -= count_bits(board->knights & board->occupied_black) * KNIGHT_VALUE;
    score -= count_bits(board->bishops & board->occupied_black) * BISHOP_VALUE;
    score -= count_bits(board->rooks & board->occupied_black) * ROOK_VALUE;
    score -= count_bits(board->queens & board->occupied_black) * QUEEN_VALUE;

    return score;
}

int evaluate_position_pieces(Chessboard *board)
{
    int score = 0;

    uint64_t bit;
    int index;

    // Pions
    bit = board->pawns;
    while (bit)
    {
        index = __builtin_ctzll(bit); // position du bit à 1 le moins significatif
        if (board->occupied_white & (1ULL << index))
            score += pawn_table[index];
        else
            score -= pawn_table[index];
        bit &= bit - 1; // supprime le bit le moins significatif
    }

    bit = board->knights;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += knight_table[index];
        else
            score -= knight_table[index];
        bit &= bit - 1;
    }

    bit = board->bishops;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += bishop_table[index];
        else
            score -= bishop_table[index];
        bit &= bit - 1;
    }

    bit = board->rooks;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += rook_table[index];
        else
            score -= rook_table[index];
        bit &= bit - 1;
    }

    bit = board->queens;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += queen_table[index];
        else
            score -= queen_table[index];
        bit &= bit - 1;
    }

    bit = board->kings;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += king_table[index];
        else
            score -= king_table[index];
        bit &= bit - 1;
    }

    return score;
}

int evaluate_position(Chessboard *board)
{
    int score = count_material(board);
    score += evaluate_position_pieces(board);
    if (board->white_to_play)
        return score;
    else
        return -1 * score;
}

int lire_piece_square_table(const char *nom_fichier, int valeurs[BOARD_SIZE])
{
    FILE *f = fopen(nom_fichier, "r");
    if (!f)
    {
        perror("Erreur ouverture fichier");
        return -1;
    }

    int count = 0;
    while (count < BOARD_SIZE && !feof(f))
    {
        if (fscanf(f, "%d", &valeurs[count]) != 1)
        {
            fprintf(stderr, "Erreur lecture valeur à l'indice %d\n", count);
            fclose(f);
            return -1;
        }
        count++;
    }

    fclose(f);

    if (count != BOARD_SIZE)
    {
        fprintf(stderr, "Le fichier doit contenir 64 valeurs, trouvé %d\n", count);
        return -1;
    }

    return 0; // succès
}

int initialize_tables()
{
    if (lire_piece_square_table("assets/squares_pieces_tables/pawn.txt", pawn_table) != 0)
        return -1;
    if (lire_piece_square_table("assets/squares_pieces_tables/knight.txt", knight_table) != 0)
        return -1;
    if (lire_piece_square_table("assets/squares_pieces_tables/bishop.txt", bishop_table) != 0)
        return -1;
    if (lire_piece_square_table("assets/squares_pieces_tables/rook.txt", rook_table) != 0)
        return -1;
    if (lire_piece_square_table("assets/squares_pieces_tables/queen.txt", queen_table) != 0)
        return -1;
    if (lire_piece_square_table("assets/squares_pieces_tables/king.txt", king_table) != 0)
        return -1;

    return 0;
}