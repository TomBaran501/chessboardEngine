#include "move.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void initialise_empty_move(Move *move)
{
    move->flag = 0;
    move->from = 0;
    move->to = 0;
    move->promotion_flag = 0;
    move->piece_taken = 0;
    move->roque_broken = 0;
    move->en_passant = 0;
}

void initialise_move(Move *move, int from, int to)
{
    move->flag = 0;
    move->from = from;
    move->to = to;
    move->promotion_flag = 0;
    move->piece_taken = 0;
    move->roque_broken = 0;
    move->en_passant = 0;
}

bool get_long_castle(Move move)
{
    return (move.flag & 1) == 1;
}
void set_long_castle(Move *move, bool value)
{
    if (value)
    {
        move->flag |= 1;
    }
    else
    {
        move->flag &= ~1;
    }
}

bool get_short_castle(Move move)
{
    return ((move.flag >> 1) & 1) == 1;
}
void set_short_castle(Move *move, bool value)
{
    if (value)
    {
        move->flag |= (1 << 1);
    }
    else
    {
        move->flag &= ~(1 << 1);
    }
}

bool get_pawn_advanced2(Move move)
{
    return ((move.flag >> 2) & 1) == 1;
}
void set_pawn_advanced2(Move *move, bool value)
{
    if (value)
    {
        move->flag |= (1 << 2);
    }
    else
    {
        move->flag &= ~(1 << 2);
    }
}

bool get_enpassant(Move move)
{
    return ((move.flag >> 3) & 1) == 1;
}

void set_enpassant(Move *move, bool value)
{
    if (value)
    {
        move->flag |= (1 << 3);
    }
    else
    {
        move->flag &= ~(1 << 3);
    }
}

/// Convertit une case en index (0 = a1, 63 = h8)
int square_to_index(char file, char rank)
{
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        return -1;
    return (7 - rank + '1') * 8 + (file - 'a');
}

void index_to_square(int index, char square[3])
{
    if (index < 0 || index > 63)
    {
        square[0] = '?';
        square[1] = '?';
        square[2] = '\0';
        return;
    }

    int f = index % 8; // colonne (0=a ... 7=h)
    int r = index / 8; // ligne (0 = rangée 1)

    square[0] = 'a' + f;
    square[1] = '8' - r;
    square[2] = '\0';
}

Move get_move(char move[10])
{
    Move m = {0};

    if (strlen(move) < 4)
        return m;

    int from = square_to_index(move[0], move[1]);
    int to = square_to_index(move[2], move[3]);

    if (from < 0 || to < 0)
        return m;

    m.from = from;
    m.to = to;
    m.flag = 0;
    m.promotion_flag = NORMAL_MOVE;

    // Gestion de la promotion : e.g., e7e8q
    if (strlen(move) == 5)
    {
        char promo = tolower(move[4]);
        switch (promo)
        {
        case 'n':
            m.promotion_flag = PROMOTION_N;
            break;
        case 'b':
            m.promotion_flag = PROMOTION_B;
            break;
        case 'r':
            m.promotion_flag = PROMOTION_R;
            break;
        case 'q':
            m.promotion_flag = PROMOTION_Q;
            break;
        }
    }

    return m;
}

void print_move(Move *move)
{
    char f[3];
    char t[3];
    char p = ' ';
    index_to_square(move->from, f);
    index_to_square(move->to, t);

    if (move->promotion_flag == PROMOTION_B)
        p = 'b';
    if (move->promotion_flag == PROMOTION_N)
        p = 'n';
    if (move->promotion_flag == PROMOTION_Q)
        p = 'q';
    if (move->promotion_flag == PROMOTION_R)
        p = 'r';

    printf("%s%s%c", f, t, p);
}