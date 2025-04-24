#include "chessboard.h"
#include "bitboard.h"
#include "masks.h"
#include "move.h"
#include "chessboardcontroller.h"

#include "stdlib.h"

int main()
{
    init_bitboards();
    Chessboard c;
    MoveList *moves = createMoveList(5);
    init_chessboard(&c);
    moves = getlegalmoves(1, &c);
    for (int i = 0; i < moves -> count; i++){
         Move m = getMove(moves,i);
        print_move(&m);
    }
    return 0;
}