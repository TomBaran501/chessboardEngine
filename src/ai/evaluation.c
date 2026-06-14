#include "evaluation.h"

int pawn_table[BOARD_SIZE];
int knight_table[BOARD_SIZE];
int bishop_table[BOARD_SIZE];
int rook_table[BOARD_SIZE];
int queen_table[BOARD_SIZE];
int king_table[BOARD_SIZE];
int king_table_endgame[BOARD_SIZE];

int ally_color;

static int distance_between_kings(Chessboard *board)
{
    int white_king = get_lsb_index(board->kings & board->occupied_white);
    int black_king = get_lsb_index(board->kings & board->occupied_black);

    int horizontal_dst = abs(white_king % 8 - black_king % 8);
    int vertical_dst = abs(white_king / 8 - black_king / 8);

    return horizontal_dst + vertical_dst;
}

int center_distance(int square)
{
    int file = square % 8;
    int rank = square / 8;

    return abs(file - 3) + abs(rank - 3);
}

static int mirror_square(int index)
{
    int rank = index / 8;
    int file = index % 8;

    return (7 - rank) * 8 + file;
}

static int count_pieces_material(Chessboard *board)
{
    int material = count_bits(board->knights) * KNIGHT_VALUE;
    material += count_bits(board->bishops) * BISHOP_VALUE;
    material += count_bits(board->rooks) * ROOK_VALUE;
    material += count_bits(board->queens) * QUEEN_VALUE;

    return material;
}

static int count_material(Chessboard *board, int color)
{
    uint64_t pieces = color == WHITE ? board->occupied_white : board->occupied_black;

    int material = count_bits(board->pawns & pieces) * PAWN_VALUE;
    material += count_bits(board->knights & pieces) * KNIGHT_VALUE;
    material += count_bits(board->bishops & pieces) * BISHOP_VALUE;
    material += count_bits(board->rooks & pieces) * ROOK_VALUE;
    material += count_bits(board->queens & pieces) * QUEEN_VALUE;

    return material;
}

static int evaluate_position_ally_pieces_endgame(Chessboard *board)
{
    int score = 0;

    uint64_t bit;
    int index;

    bit = board->kings;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += king_table_endgame[index];
        else
            score -= king_table_endgame[mirror_square(index)];
        bit &= bit - 1;
    }

    return score;
}

static int evaluate_position_ally_pieces(Chessboard *board)
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
            score -= pawn_table[mirror_square(index)];
        bit &= bit - 1;
    }

    bit = board->knights;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += knight_table[index];
        else
            score -= knight_table[mirror_square(index)];
        bit &= bit - 1;
    }

    bit = board->bishops;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += bishop_table[index];
        else
            score -= bishop_table[mirror_square(index)];
        bit &= bit - 1;
    }

    bit = board->rooks;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += rook_table[index];
        else
            score -= rook_table[mirror_square(index)];
        bit &= bit - 1;
    }

    bit = board->queens;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += queen_table[index];
        else
            score -= queen_table[mirror_square(index)];
        bit &= bit - 1;
    }

    bit = board->kings;
    while (bit)
    {
        index = __builtin_ctzll(bit);
        if (board->occupied_white & (1ULL << index))
            score += king_table[index];
        else
            score -= king_table[mirror_square(index)];
        bit &= bit - 1;
    }

    return score;
}

int evaluate_king_safety(Chessboard *board, int white_material, int black_material)
{
    if (abs(white_material - black_material) < ROOK_VALUE)
        return 0;

    uint64_t week_king = white_material > black_material ? board->kings & board->occupied_black : board->kings & board->occupied_white;
    
    int score = (14 - distance_between_kings(board)) * 4;
    score += center_distance(get_lsb_index(week_king)) * 10;

    return white_material > black_material ? score : -score;
}

int evaluate_position(Chessboard *board)
{
    int white_material = count_material(board, WHITE);
    int black_material = count_material(board, BLACK);
    int score = white_material - black_material;
    int value_pieces = count_pieces_material(board);

    score += (evaluate_position_ally_pieces(board) * value_pieces + evaluate_position_ally_pieces_endgame(board) * (VALUE_PIECES_START - value_pieces)) / VALUE_PIECES_START;
    score += evaluate_king_safety(board, white_material, black_material);

    if (!board->white_to_play)
        score = -score;

    return score;
}

static int lire_piece_square_table(const char *nom_fichier, int valeurs[BOARD_SIZE])
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
    int color_ai = 0;
    ally_color = color_ai;
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
    if (lire_piece_square_table("assets/squares_pieces_tables/king_endgame.txt", king_table_endgame) != 0)
        return -1;

    return 0;
}

int get_piece_value(Chessboard *board, int square)
{
    uint64_t pos = create_1bit_board(square);

    if (board->pawns & pos)
        return PAWN_VALUE;

    if (board->queens & pos)
        return QUEEN_VALUE;

    if (board->rooks & pos)
        return ROOK_VALUE;

    if (board->bishops & pos)
        return BISHOP_VALUE;

    if (board->knights & pos)
        return KNIGHT_VALUE;

    return 0;
}