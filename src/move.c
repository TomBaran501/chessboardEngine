#include "move.h"

#include <stdio.h>


void initialise_empty_move(Move *move){
    move -> flag = 0;
    move -> from = 0;
    move -> to = 0;
}

void print_move(Move *move)
{
    printf("Move from %d to %d with flag %d\n", move -> from, move -> to, move -> flag);
}

bool get_long_castle(Move move)
{
    return (move.flag & 1) == 1;
}
void set_long_castle(Move *move, bool value)
{
    if (value)
    {
        move -> flag |= 1;
    }
    else
    {
        move -> flag &= ~1;
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
        move -> flag |= (1 << 1);
    }
    else
    {
        move -> flag &= ~(1 << 1);
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
        move -> flag |= (1 << 2);
    }
    else
    {
        move -> flag &= ~(1 << 2);
    }
}

bool get_promotion(Move move)
{
    return ((move.flag >> 3) & 1) == 1;
}
void set_promotion(Move *move, bool value)
{
    if (value)
    {
        move -> flag |= (1 << 3);
    }
    else
    {
        move -> flag &= ~(1 << 3);
    }
}