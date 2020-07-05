// Bitboard representation of the chessboard:
// It's an array of 12 64-bit integers, each bit representing if a piece is found in that position, specifically the LSB represents the A1 square, then the second represents A2, etc ...

#ifndef _CHESSBOARD_INCLUDED
#define _CHESSBOARD_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Move.h"

typedef struct {
	uint64_t bitBoard[12];
	SDL_Surface *spriteMap;
} Chessboard;


extern Chessboard *Chessboard_create();
extern void Chessboard_draw(Chessboard *, SDL_Surface *);
extern MovesArray *Chessboard_computePieceMoves(Chessboard *, uint8_t pieceLocation);

#endif
