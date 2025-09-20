#include "evaluation.h"

int pawn_table[BOARD_SIZE];
int knight_table[BOARD_SIZE];
int bishop_table[BOARD_SIZE];
int rook_table[BOARD_SIZE];
int queen_table[BOARD_SIZE];
int king_table[BOARD_SIZE];

int ally_color = BLACK;

int abs(int x)
{
    if (x >= 0)
        return x;
    return -x;
}

int max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

int count_material(Chessboard *board, int *total_value)
{
    int score = 0;
    *total_value = 0;

    // Matériel blanc

    score += count_bits(board->knights & board->occupied_white) * KNIGHT_VALUE;
    score += count_bits(board->bishops & board->occupied_white) * BISHOP_VALUE;
    score += count_bits(board->rooks & board->occupied_white) * ROOK_VALUE;
    score += count_bits(board->queens & board->occupied_white) * QUEEN_VALUE;

    total_value += score;

    // Matériel noir (on soustrait)

    score -= count_bits(board->knights & board->occupied_black) * KNIGHT_VALUE;
    score -= count_bits(board->bishops & board->occupied_black) * BISHOP_VALUE;
    score -= count_bits(board->rooks & board->occupied_black) * ROOK_VALUE;
    score -= count_bits(board->queens & board->occupied_black) * QUEEN_VALUE;

    *total_value += *total_value - score;

    score += count_bits(board->pawns & board->occupied_white) * PAWN_VALUE;
    score -= count_bits(board->pawns & board->occupied_black) * PAWN_VALUE;
    return score;
}

int evaluate_attack_king(Chessboard *board)
{
    int score = 0;

    uint64_t our_king_bb = board->white_to_play ? board->kings & board->occupied_white : board->kings & board->occupied_black;
    uint64_t enemy_king_bb = board->white_to_play ? board->kings & board->occupied_black : board->kings & board->occupied_white;
    int color = board->white_to_play ? WHITE : BLACK;

    int our_king_index = get_lsb_index(our_king_bb);
    int enemy_king_index = get_lsb_index(enemy_king_bb);

    int our_king_rank = our_king_index / 8;
    int our_king_file = our_king_index % 8;
    int enemy_king_rank = enemy_king_index / 8;
    int enemy_king_file = enemy_king_index % 8;

    int malus = max(our_king_rank - 4, 3 - our_king_rank) + max(our_king_file - 4, 3 - our_king_file);
    int bonus = max(enemy_king_rank - 4, 3 - enemy_king_rank) + max(enemy_king_file - 4, 3 - enemy_king_file);

    score = bonus - malus;

    if (color == ally_color)
    {
        int dst = abs(our_king_file - enemy_king_file) + abs(our_king_rank - enemy_king_rank);
        score += (14 - dst);
    }

    if (color == WHITE)
        return score * 10;
    return -1 * score * 10;
}

int evaluate_position_ally_pieces(Chessboard *board)
{
    int score = 0;

    uint64_t bit;
    int index;

    // Pions
    bit = board->pawns;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += pawn_table[index];
        else
        {
            int rank = index / 8;
            int file = index % 8;
            score -= pawn_table[(7 - rank) * 8 + file];
        }
        bit &= bit - 1;
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
    int total_value = 0;
    int score = count_material(board, &total_value);
    float endgame_phase = 1.0 - ((float)total_value / (float)MAX_VALUE);
    if (endgame_phase > 1.0)
        endgame_phase = 1.0;

    score += (int)((1.0f - endgame_phase) * evaluate_position_ally_pieces(board) + endgame_phase * evaluate_attack_king(board));
    if (board->white_to_play)
        return score;
    else
        return -1 * score;
}

int lire_piece_square_table(const char *nom_fichier, int valeurs[BOARD_SIZE])
{
    printf("fichier: %s\n", nom_fichier);
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
