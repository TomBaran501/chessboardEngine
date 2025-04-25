#include "chessboard.h"
#include "masks.h"
#include "movelist.h"
#include "bitboard.h"

MoveList *getlegalmoves(int piecePos, Chessboard *chessboard);
bool play_move(Chessboard *board, Move move);

void update_bitboards(uint64_t from_bitboard, Chessboard *board, uint64_t to_bitboard);
