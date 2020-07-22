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
extern MovesArray *Chessboard_computePieceMoves(Chessboard *, uint8_t pieceLocation, bool checkCastling, bool checkNextMoveKingInCheck);
extern bool Chessboard_isHighlightable(Chessboard *, uint8_t pieceLocation, uint8_t turn);
extern void Chessboard_applyMove(Chessboard *, Move);
extern bool Chessboard_squareAttacked(Chessboard *, uint8_t attacker, uint8_t squareLocation);
extern bool Chessboard_kingInCheck(Chessboard *, uint8_t kingType);
extern void Chessboard_destroy(Chessboard *);
extern bool Chessboard_isMoveLegal(Chessboard *, Move);
extern bool Chessboard_isCheckmate(Chessboard *, uint8_t kingType);
extern bool Chessboard_isStalemate(Chessboard *);
extern bool Chessboard_hasLegalMoves(Chessboard *, uint8_t attacker);
extern MovesArray *Chessboard_computeAllMoves(Chessboard *, uint8_t side, bool checkCastling, bool checkNextMoveKingInCheck);
extern uint8_t Chessboard_countPieces(Chessboard *, uint8_t pieceType, uint8_t side);
extern uint64_t Chessboard_getPieceMask(Chessboard *, uint8_t pieceType, uint8_t side);

#endif
