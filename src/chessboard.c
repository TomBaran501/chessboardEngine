#include "chessboard.h"

#include "stdlib.h"

// Position de départ des pièces en notation bitboard
#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

void init_chessboard(Chessboard *board) {

    board->pawns = RANK_2 | RANK_7;
    board->rooks = 0x8100000000000081ULL;
    board->knights = 0x4200000000000042ULL;
    board->bishops = 0x2400000000000024ULL;
    board->queens = 0x0800000000000008ULL;
    board->kings = 0x1000000000000010ULL;
    
    board->occupied_white = RANK_1 | RANK_2;
    board->occupied_black = RANK_7 | RANK_8;
}

/// @brief This function prints the chessboard
/// @param board the chessboard to print
void print_chessboard(const Chessboard *board) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            uint64_t pos = 1ULL << (rank * 8 + file);
            if (board->pawns & board->occupied_white & pos) printf("P ");
            else if (board->knights & board->occupied_white & pos) printf("N ");
            else if (board->bishops & board->occupied_white & pos) printf("B ");
            else if (board->rooks & board->occupied_white & pos) printf("R ");
            else if (board->queens & board->occupied_white & pos) printf("Q ");
            else if (board->kings & board->occupied_white & pos) printf("K ");

            else if (board->pawns & board->occupied_black & pos) printf("p ");
            else if (board->knights & board->occupied_black & pos) printf("n ");
            else if (board->bishops & board->occupied_black & pos) printf("b ");
            else if (board->rooks & board->occupied_black & pos) printf("r ");
            else if (board->queens & board->occupied_black & pos) printf("q ");
            else if (board->kings & board->occupied_black & pos) printf("k ");

            else printf(". ");
        }
        printf("\n");
    }
    printf("\n");
}
