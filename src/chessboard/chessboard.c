#include "chessboard.h"

#include "stdlib.h"

// Position de départ des pièces en notation bitboard
#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

int casttling_squares[4] = {62, 58, 6, 2};
const char *castling_rights = "KQkq";


void init_chessboard(Chessboard *board)
{

    board->pawns = RANK_2 | RANK_7;
    board->rooks = 0x8100000000000081ULL;
    board->knights = 0x4200000000000042ULL;
    board->bishops = 0x2400000000000024ULL;
    board->queens = 0x0800000000000008ULL;
    board->kings = 0x1000000000000010ULL;

    board->occupied_black = RANK_1 | RANK_2;
    board->occupied_white = RANK_7 | RANK_8;

    board->white_to_play = true;
}


void print_chessboard(const Chessboard *board)
{
    for (int rank = 0; rank <= 7; rank++)
    {
        printf("%i  ", 8 - rank);
        for (int file = 0; file < 8; file++)
        {
            uint64_t pos = 1ULL << (rank * 8 + file);
            if (board->pawns & board->occupied_white & pos)
                printf("P ");
            else if (board->knights & board->occupied_white & pos)
                printf("N ");
            else if (board->bishops & board->occupied_white & pos)
                printf("B ");
            else if (board->rooks & board->occupied_white & pos)
                printf("R ");
            else if (board->queens & board->occupied_white & pos)
                printf("Q ");
            else if (board->kings & board->occupied_white & pos)
                printf("K ");

            else if (board->pawns & board->occupied_black & pos)
                printf("p ");
            else if (board->knights & board->occupied_black & pos)
                printf("n ");
            else if (board->bishops & board->occupied_black & pos)
                printf("b ");
            else if (board->rooks & board->occupied_black & pos)
                printf("r ");
            else if (board->queens & board->occupied_black & pos)
                printf("q ");
            else if (board->kings & board->occupied_black & pos)
                printf("k ");

            else
                printf(". ");
        }
        printf("\n");
    }
    printf("\n");
    printf("   a b c d e f g h");
    printf("\n \n");
}

char *return_fen_code(Chessboard *board)
{
    char *fen = malloc(100); // Assez grand pour contenir le FEN
    fen[0] = '\0';

    char piece_char[64] = {0};
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t mask = 1ULL << sq;
        if (board->pawns & mask)
            piece_char[sq] = (board->occupied_white & mask) ? 'P' : 'p';
        else if (board->knights & mask)
            piece_char[sq] = (board->occupied_white & mask) ? 'N' : 'n';
        else if (board->bishops & mask)
            piece_char[sq] = (board->occupied_white & mask) ? 'B' : 'b';
        else if (board->rooks & mask)
            piece_char[sq] = (board->occupied_white & mask) ? 'R' : 'r';
        else if (board->queens & mask)
            piece_char[sq] = (board->occupied_white & mask) ? 'Q' : 'q';
        else if (board->kings & mask)
            piece_char[sq] = (board->occupied_white & mask) ? 'K' : 'k';
        else
            piece_char[sq] = '1';
    }

    // Générer la partie de la position
    for (int rank = 7; rank >= 0; rank--)
    {
        int empty = 0;
        for (int file = 0; file < 8; file++)
        {
            char c = piece_char[rank * 8 + file];
            if (c == '1')
            {
                empty++;
            }
            else
            {
                if (empty > 0)
                {
                    char tmp[2];
                    sprintf(tmp, "%d", empty);
                    strcat(fen, tmp);
                    empty = 0;
                }
                strncat(fen, &c, 1);
            }
        }
        if (empty > 0)
        {
            char tmp[2];
            sprintf(tmp, "%d", empty);
            strcat(fen, tmp);
        }
        if (rank > 0)
            strcat(fen, "/");
    }

    // Tour de jeu
    strcat(fen, board->white_to_play ? " w " : " b ");

    // Roques
    int has_castle = 0;
    if (board->castling & create_1bit_board(casttling_squares[0]))
    {
        strcat(fen, "K");
        has_castle = 1;
    }
    if (board->castling & create_1bit_board(casttling_squares[1]))
    {
        strcat(fen, "Q");
        has_castle = 1;
    }
    if (board->castling & create_1bit_board(casttling_squares[2]))
    {
        strcat(fen, "k");
        has_castle = 1;
    }
    if (board->castling & create_1bit_board(casttling_squares[3]))
    {
        strcat(fen, "q");
        has_castle = 1;
    }
    if (!has_castle)
        strcat(fen, "-");

    strcat(fen, " ");

    // En passant
    if (board->enpassant)
    {
        int ep_index = __builtin_ctzll(board->enpassant); // index 0-63
        char ep[3];
        ep[0] = 'a' + (ep_index % 8);
        ep[1] = '8' - (ep_index / 8);
        ep[2] = '\0';
        strcat(fen, ep);
    }
    else
    {
        strcat(fen, "-");
    }

    // Pas de demi-coup ou de fullmove ici
    strcat(fen, " 0 1");

    return fen;
}

void init_chessboard_from_fen(Chessboard *board, const char *fen)
{
    memset(board, 0, sizeof(Chessboard)); // Tout mettre à zéro

    int square = 56; // Commence en haut à gauche (rank 8, file a)
    const char *ptr = fen;

    // Lecture des pièces
    while (*ptr && *ptr != ' ')
    {
        if (*ptr == '/')
        {
            square -= 16; // Passe au rang inférieur
        }
        else if (isdigit(*ptr))
        {
            square += *ptr - '0'; // Sauter les cases vides
        }
        else
        {
            uint64_t bit = 1ULL << square;
            switch (*ptr)
            {
            case 'P':
                board->pawns |= bit;
                board->occupied_white |= bit;
                break;
            case 'N':
                board->knights |= bit;
                board->occupied_white |= bit;
                break;
            case 'B':
                board->bishops |= bit;
                board->occupied_white |= bit;
                break;
            case 'R':
                board->rooks |= bit;
                board->occupied_white |= bit;
                break;
            case 'Q':
                board->queens |= bit;
                board->occupied_white |= bit;
                break;
            case 'K':
                board->kings |= bit;
                board->occupied_white |= bit;
                break;
            case 'p':
                board->pawns |= bit;
                board->occupied_black |= bit;
                break;
            case 'n':
                board->knights |= bit;
                board->occupied_black |= bit;
                break;
            case 'b':
                board->bishops |= bit;
                board->occupied_black |= bit;
                break;
            case 'r':
                board->rooks |= bit;
                board->occupied_black |= bit;
                break;
            case 'q':
                board->queens |= bit;
                board->occupied_black |= bit;
                break;
            case 'k':
                board->kings |= bit;
                board->occupied_black |= bit;
                break;
            }
            square++;
        }
        ptr++;
    }

    // Tour de jeu
    while (*ptr == ' ')
        ptr++;
    board->white_to_play = (*ptr == 'w') ? 1 : 0;

    // Roques
    board->castling = 0;
    ptr += 2;
    if (*ptr == '-')
    {
        ptr++;
    }
    else
    {
        while (*ptr && *ptr != ' ')
        {
            switch (*ptr)
            {
            case 'K':
                board->castling |= create_1bit_board(casttling_squares[0]);
                break;
            case 'Q':
                board->castling |= create_1bit_board(casttling_squares[1]);
                break;
            case 'k':
                board->castling |= create_1bit_board(casttling_squares[2]);
                break;
            case 'q':
                board->castling |= create_1bit_board(casttling_squares[3]);
                break;
            }
            ptr++;
        }
    }

    // En passant
    while (*ptr == ' ')
        ptr++;
    if (*ptr == '-')
    {
        board->enpassant = 0;
        ptr++;
    }
    else
    {
        int file = ptr[0] - 'a';
        int rank = ptr[1] - '1';
        int index = (7 - rank) * 8 + file;
        board->enpassant = 1ULL << index;
        ptr += 2;
    }

    // (Optionnel) ignore les demi-coups et fullmove
    init_bitboards();
}
