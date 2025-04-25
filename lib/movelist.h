#include "move.h"

#include "stdlib.h"

typedef struct
{
    Move *moves;  // tableau dynamique de coups
    int count;    // nombre actuel de coups
    int capacity; // capacité allouée
} MoveList;

MoveList *createMoveList(int initialCapacity);
void addMove(MoveList *list, Move move);
Move getMove(const MoveList *list, int i);
void freeMoveList(MoveList *list);
bool ismoveinlist(const MoveList *list, Move move);