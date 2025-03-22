#include "chessboard.h"
#include "move.h"

#include "stdlib.h"

int main()
{

    Chessboard *chessboard = malloc(sizeof(Chessboard));
    init_chessboard(chessboard);
    print_chessboard(chessboard);
    free(chessboard);

    Move *move = malloc(sizeof(Move));
    initialise_empty_move(move);
    set_long_castle(move, true);
    set_short_castle(move, true);
    print_move(move);
}