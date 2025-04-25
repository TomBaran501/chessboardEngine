#include "movelist.h"

MoveList *createMoveList(int initialCapacity)
{
    MoveList *list = malloc(sizeof(MoveList));
    list->moves = malloc(initialCapacity * sizeof(Move));
    list->count = 0;
    list->capacity = initialCapacity;
    return list;
}

void addMove(MoveList *list, Move move)
{
    if (list->count >= list->capacity)
    {
        list->capacity *= 2;
        list->moves = realloc(list->moves, list->capacity * sizeof(Move));
    }
    list->moves[list->count++] = move;
}

Move getMove(const MoveList *list, int i)
{
    if (i < 0 || i >= list->count)
    {
        Move move;
        initialise_empty_move(&move);
        return move;
    }
    return list->moves[i];
}

void freeMoveList(MoveList *list)
{
    free(list->moves);
    free(list);
}

bool ismoveinlist(const MoveList *list, Move move)
{
    for (int i = 0; i < list->count; i++)
    {
        if (list->moves[i].from == move.from && list->moves[i].to == move.to)
        {
            return true;
        }
    }
    return false;
}
