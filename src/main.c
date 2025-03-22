#include "chessboard.h"

#include "stdlib.h"

int main()
{
    
    Chessboard *chessboard = malloc(sizeof(Chessboard));;
    init_chessboard(chessboard);
    print_chessboard(chessboard);
    free(chessboard);

}