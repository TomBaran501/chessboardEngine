#include "bitboard.h"

static inline int rank_of(int sq) { return sq / 8; }
static inline int file_of(int sq) { return sq % 8; }

unsigned int state = 1884258747;

uint64_t DIAG_MASKS[15];
uint64_t ANTI_DIAG_MASKS[15];

static const uint64_t RANK_MASKS[8] = {
    0x00000000000000FFULL,
    0x000000000000FF00ULL,
    0x0000000000FF0000ULL,
    0x00000000FF000000ULL,
    0x000000FF00000000ULL,
    0x0000FF0000000000ULL,
    0x00FF000000000000ULL,
    0xFF00000000000000ULL};

static const uint64_t FILE_MASKS[8] = {
    0x0101010101010101ULL,
    0x0202020202020202ULL,
    0x0404040404040404ULL,
    0x0808080808080808ULL,
    0x1010101010101010ULL,
    0x2020202020202020ULL,
    0x4040404040404040ULL,
    0x8080808080808080ULL};

uint64_t create_1bit_board(int exp)
{
    return (1ULL << exp);
}

uint64_t set_bit(uint64_t bitboard, int square)
{
    return bitboard | (1ULL << square);
}

void print_bitboard(uint64_t bitboard)
{
    for (int rank = 0; rank < 8; rank++)
    {
        printf("%i  ", 8 - rank);
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            printf("%c ", (bitboard & (1ULL << square)) ? '1' : '.');
        }
        printf("\n");
    }
    printf("\n");
    printf("   a b c d e f g h");
    printf("\n \n");
}

uint64_t pop_bit(uint64_t bb)
{
    if (bb == 0)
        return -1;
    bb &= bb - 1;
    return bb;
}

unsigned int get_random_number()
{
    unsigned int num = state;
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;
    state = num;
    return num;
}

uint64_t get_random_bitboard()
{
    uint64_t n1, n2, n3, n4;

    n1 = (uint64_t)(get_random_number() & 0xFFFF);
    n2 = (uint64_t)(get_random_number() & 0xFFFF);
    n3 = (uint64_t)(get_random_number() & 0xFFFF);
    n4 = (uint64_t)(get_random_number() & 0xFFFF);

    return n1 | (n2 << 16) | (n2 << 16) | (n3 << 32) | (n4 << 48);
}



void init_diag_masks() {
    for (int d = -7; d <= 7; d++) {
        uint64_t mask = 0ULL;
        for (int r = 0; r < 8; r++) {
            int f = r - d;
            if (f >= 0 && f < 8) {
                mask |= 1ULL << (r * 8 + f);
            }
        }
        DIAG_MASKS[d + 7] = mask;
    }

    for (int d = 0; d <= 14; d++) {
        uint64_t mask = 0ULL;
        for (int r = 0; r < 8; r++) {
            int f = d - r;
            if (f >= 0 && f < 8) {
                mask |= 1ULL << (r * 8 + f);
            }
        }
        ANTI_DIAG_MASKS[d] = mask;
    }
}

uint64_t line_mask(int sq1, int sq2)
{
    int r1 = rank_of(sq1), f1 = file_of(sq1);
    int r2 = rank_of(sq2), f2 = file_of(sq2);

    if (sq1 == sq2)
        return 0ULL;

    if (r1 == r2)
    {
        return RANK_MASKS[r1];
    }
    if (f1 == f2)
    {
        return FILE_MASKS[f1];
    }
    if ((r1 - f1) == (r2 - f2))
    {
        return DIAG_MASKS[(r1 - f1) + 7];
    }
    if ((r1 + f1) == (r2 + f2))
    {
        return ANTI_DIAG_MASKS[r1 + f1];
    }
    return 0ULL;
}
