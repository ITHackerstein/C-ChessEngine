#ifndef _CHESSAI_INCLUDED
#define _CHESSAI_INCLUDED

#include "Move.h"
#include "Chessboard.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MOVE_GENERATION_DEPTH 3

static int32_t ChessAI_evaluate(Chessboard *);
static int32_t ChessAI_MiniMax(Chessboard *, uint8_t depth, int32_t alpha, int32_t beta, uint8_t movingSide);

extern void ChessAI_nextMove(Chessboard *);

#endif
