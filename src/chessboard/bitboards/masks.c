#include "masks.h"

// Masque pour les pieces non-linéaire
uint64_t masks_king_moves[BOARD_SIZE];
uint64_t masks_knight_moves[64];
uint64_t masks_pawn_captures[2][BOARD_SIZE];
uint64_t masks_pawn_moves[2][BOARD_SIZE];

// Masques pour les pieces linéaires
uint64_t bishop_masks[64];
uint64_t rook_masks[64];
uint64_t bishop_attacks[64][512];
uint64_t rook_attacks[64][4096];

// Bit du nombre de mouvements possibles par cases
const int bishop_relevant_bits[64] = {
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    7,
    7,
    7,
    7,
    5,
    5,
    5,
    5,
    7,
    9,
    9,
    7,
    5,
    5,
    5,
    5,
    7,
    9,
    9,
    7,
    5,
    5,
    5,
    5,
    7,
    7,
    7,
    7,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    5,
    6,
    5,
    5,
    5,
    6,
    5,
    5,
    6,
};

const int rook_relevant_bits[64] = {
    12,
    11,
    11,
    11,
    11,
    11,
    11,
    12,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    12,
    11,
    11,
    11,
    11,
    11,
    11,
    12,
};

// "Magic numbers", les nombres qui permettent les indices pour les tableaux de déplacements possibles
int64_t rook_magic_numbers[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL};

int64_t bishop_magic_numbers[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL};

/// @brief Génère le mask des coups possibles pour le roi
void init_king_masks()
{
    uint64_t bitmask;
    for (int num_case = 0; num_case < BOARD_SIZE; num_case++)
    {
        bitmask = 0;
        if (num_case % 8 != 0)
        {
            bitmask |= create_1bit_board(num_case - 1);
            bitmask |= (num_case > 55) ? 0 : create_1bit_board(num_case + 7);
            bitmask |= (num_case < 8) ? 0 : create_1bit_board(num_case - 9);
        }
        if (num_case % 8 != 7)
        {
            bitmask |= create_1bit_board(num_case + 1);
            bitmask |= (num_case > 55) ? 0 : create_1bit_board(num_case + 9);
            bitmask |= (num_case < 8) ? 0 : create_1bit_board(num_case - 7);
        }

        bitmask |= (num_case < 8) ? 0 : create_1bit_board(num_case - 8);
        bitmask |= (num_case > 55) ? 0 : create_1bit_board(num_case + 8);

        masks_king_moves[num_case] = bitmask;
    }
}

/// @brief Génere le masque des coups de captures possibles pour les pions
void init_pawn_capture_masks()
{
    uint64_t bitmask_white;
    uint64_t bitmask_black;
    for (int num_case = 0; num_case < 63; num_case++)
    {
        if (num_case > 7)
        {
            bitmask_white = 0;
            bitmask_white |= (num_case % 8 == 0) ? 0 : create_1bit_board(num_case - 9);
            bitmask_white |= (num_case % 8 == 7) ? 0 : create_1bit_board(num_case - 7);
        }

        if (num_case < 56)
        {
            bitmask_black = 0;
            bitmask_black |= (num_case % 8 == 0) ? 0 : create_1bit_board(num_case + 7);
            bitmask_black |= (num_case % 8 == 7) ? 0 : create_1bit_board(num_case + 9);
        }

        masks_pawn_captures[WHITE][num_case] = bitmask_white;
        masks_pawn_captures[BLACK][num_case] = bitmask_black;
    }
}

/// @brief Génere le masque des coups possibles pour les pions
void init_pawn_moves_masks()
{
    uint64_t bitmask_white;
    uint64_t bitmask_black;
    for (int num_case = 8; num_case < 56; num_case++)
    {
        bitmask_white = 0;
        bitmask_white |= create_1bit_board(num_case - 8);
        bitmask_white |= (num_case / 8 == 6) ? create_1bit_board(num_case - 16) : 0;

        bitmask_black = 0;
        bitmask_black |= create_1bit_board(num_case + 8);
        bitmask_black |= (num_case / 8 == 1) ? create_1bit_board(num_case + 16) : 0;

        masks_pawn_moves[WHITE][num_case] = bitmask_white;
        masks_pawn_moves[BLACK][num_case] = bitmask_black;
    }
}

/// @brief Génere le masque des coups possibles pour les cavaliers
void init_knight_masks()
{
    uint64_t bitmask;
    for (int num_case = 0; num_case < BOARD_SIZE; num_case++)
    {
        bitmask = 0;

        if (num_case % 8 > 1)
        {
            bitmask |= (num_case < 56) ? create_1bit_board(num_case + 6) : 0;
            bitmask |= (num_case > 7) ? create_1bit_board(num_case - 10) : 0;
        }
        if (num_case % 8 > 0)
        {
            bitmask |= (num_case < 48) ? create_1bit_board(num_case + 15) : 0;
            bitmask |= (num_case > 15) ? create_1bit_board(num_case - 17) : 0;
        }
        if (num_case % 8 < 7)
        {
            bitmask |= (num_case < 48) ? create_1bit_board(num_case + 17) : 0;
            bitmask |= (num_case > 15) ? create_1bit_board(num_case - 15) : 0;
        }
        if (num_case % 8 < 6)
        {
            bitmask |= (num_case < 56) ? create_1bit_board(num_case + 10) : 0;
            bitmask |= (num_case > 7) ? create_1bit_board(num_case - 6) : 0;
        }

        masks_knight_moves[num_case] = bitmask;
    }
}

/// @brief Génere le bitboard des coups possibles pour un fou sur une case
uint64_t mask_bishop_attacks(int square)
{
    uint64_t attacks = 0ULL;
    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
        attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
        attacks |= (1ULL << (r * 8 + f));

    return attacks;
}

/// @brief Génere le bitboard des coups possibles pour une tour sur une case
uint64_t mask_rook_attacks(int square)
{
    uint64_t attacks = 0ULL;
    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 6; r++)
        attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--)
        attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++)
        attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--)
        attacks |= (1ULL << (tr * 8 + f));

    return attacks;
}

/// @brief Génere le bitboard des coups possibles pour un fou sur une case en tenant compte des autres pieces
uint64_t bishop_attacks_on_the_fly(int sq, uint64_t blockers)
{
    uint64_t mask = 0;
    int rank = sq / 8, file = sq % 8;
    int r, f;

    for (r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++)
    {
        mask |= create_1bit_board(r * 8 + f);
        if (create_1bit_board(r * 8 + f) & blockers)
            break;
    }
    for (r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--)
    {
        mask |= create_1bit_board(r * 8 + f);
        if (create_1bit_board(r * 8 + f) & blockers)
            break;
    }
    for (r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++)
    {
        mask |= create_1bit_board(r * 8 + f);
        if (create_1bit_board(r * 8 + f) & blockers)
            break;
    }
    for (r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--)
    {
        mask |= create_1bit_board(r * 8 + f);
        if (create_1bit_board(r * 8 + f) & blockers)
            break;
    }
    return mask;
}
/// @brief Génere le bitboard des coups possibles pour une tour sur une case en tenant compte des autres pieces
uint64_t rook_attacks_on_the_fly(int sq, uint64_t blockers)
{
    uint64_t mask = 0;
    int rank = sq / 8, file = sq % 8;
    int r, f;

    for (r = rank + 1; r < 8; r++)
    {
        mask |= create_1bit_board(r * 8 + file);
        if (create_1bit_board(r * 8 + file) & blockers)
            break;
    }
    for (r = rank - 1; r >= 0; r--)
    {
        mask |= create_1bit_board(r * 8 + file);
        if (create_1bit_board(r * 8 + file) & blockers)
            break;
    }
    for (f = file + 1; f < 8; f++)
    {
        mask |= create_1bit_board(rank * 8 + f);
        if (create_1bit_board(rank * 8 + f) & blockers)
            break;
    }
    for (f = file - 1; f >= 0; f--)
    {
        mask |= create_1bit_board(rank * 8 + f);
        if (create_1bit_board(rank * 8 + f) & blockers)
            break;
    }
    return mask;
}

uint64_t set_occupancy(int index, uint64_t attack_mask)
{
    int nb_bits_in_mask = count_bits(attack_mask);
    uint64_t occ = 0;

    for (int i = 0; i < nb_bits_in_mask; i++)
    {
        int sq = get_lsb_index(attack_mask);
        attack_mask = pop_bit(attack_mask);

        if (index & (1 << i))
            occ |= create_1bit_board(sq);
    }
    return occ;
}

// init slider piece's attack tables
void init_sliders_attacks(int bishop)
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        // init current mask
        uint64_t attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);

        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++)
        {
            // bishop
            if (bishop)
            {
                // init current occupancy variation
                uint64_t occupancy = set_occupancy(index, attack_mask);

                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);

                // init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }

            // rook
            else
            {
                // init current occupancy variation
                uint64_t occupancy = set_occupancy(index, attack_mask);

                // init magic index
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);

                // init bishop attacks
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}

// get bishop attacks
uint64_t get_bishop_attacks(int square, uint64_t occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];

    // return bishop attacks
    return bishop_attacks[square][occupancy];
}

// get rook attacks
uint64_t get_rook_attacks(int square, uint64_t occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];

    // return rook attacks
    return rook_attacks[square][occupancy];
}

void init_bitboards()
{
    init_king_masks();
    init_pawn_capture_masks();
    init_pawn_moves_masks();
    init_knight_masks();

    int bishop = 1;
    int rook = 0;

    // init slider pieces attacks
    init_sliders_attacks(bishop);
    init_sliders_attacks(rook);
    init_diag_masks();
}