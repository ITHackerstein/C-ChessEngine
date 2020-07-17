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
	Move lastMove;
	SDL_Texture *spriteMap;
} Chessboard;


static uint8_t LS1B(uint64_t n);
static uint8_t MS1B(uint64_t n);

extern Chessboard *Chessboard_create(SDL_Renderer *);
extern void Chessboard_draw(Chessboard *, SDL_Renderer *, uint8_t highlightedPiece, MovesArray *highlightedPieceMoves);
extern MovesArray *Chessboard_computePieceMoves(Chessboard *, uint8_t pieceLocation, bool checkCastling);
extern bool Chessboard_isHighlightable(Chessboard *, uint8_t pieceLocation, uint8_t turn);
extern void Chessboard_applyMove(Move, Chessboard *);
extern bool Chessboard_squareAttacked(Chessboard *, uint8_t attacker, uint8_t squareLocation);
extern bool Chessboard_kingInCheck(Chessboard *, uint8_t kingType);

#endif
