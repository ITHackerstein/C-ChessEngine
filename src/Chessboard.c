#include "Chessboard.h"

Chessboard *Chessboard_create(SDL_Renderer *renderer) {
	Chessboard *chessboard = malloc(sizeof(Chessboard));
	memset(chessboard->bitBoard, 0, sizeof(uint64_t) * 12);

	chessboard->bitBoard[0] = 0xff00ull;               // White Pawn
	chessboard->bitBoard[1] = 0x81ull;                 // White Rook
	chessboard->bitBoard[2] = 0x42ull;                 // White Knight
	chessboard->bitBoard[3] = 0x24ull;                 // White Bishop
	chessboard->bitBoard[4] = 8ull;                    // White Queen
	chessboard->bitBoard[5] = 0x10ull;                 // White King
	chessboard->bitBoard[6] = 0xff000000000000ull;     // Black Pawn
	chessboard->bitBoard[7] = 0x8100000000000000ull;   // Black Rook
	chessboard->bitBoard[8] = 0x4200000000000000ull;   // Black Knight
	chessboard->bitBoard[9] = 0x2400000000000000ull;   // Black Bishop
	chessboard->bitBoard[10] = 0x800000000000000ull;   // Black Queen
	chessboard->bitBoard[11] = 0x1000000000000000ull;  // Black King

	chessboard->spriteMap = IMG_LoadTexture(renderer, "res/Chess_Pieces.png");

	return chessboard;
}

void Chessboard_draw(Chessboard *chessboard, SDL_Renderer *renderer, uint8_t highlightedPiece) {
	uint32_t w, h;
	SDL_GetRendererOutputSize(renderer, &w, &h);

	const uint32_t xScl = w / 8;
	const uint32_t yScl = h / 8;

	for (uint32_t j = 0; j < 8; ++j) {
		for (uint32_t i = 0; i < 8; ++i) {
			const SDL_Rect tileRect = {.x = xScl * i, .y = yScl * j, .w = xScl, .h = yScl};
			if ((i + j) % 2 == 0)
				SDL_SetRenderDrawColor(renderer, 0xe8, 0xeb, 0xef, 0xff);
			else
				SDL_SetRenderDrawColor(renderer, 0x7d, 0x87, 0x96, 0xff);
			SDL_RenderFillRect(renderer, &tileRect);
		}
	}

	if (highlightedPiece < 64) {
		const SDL_Rect originalRect = {.x = (highlightedPiece % 8) * xScl, .y = (7 - highlightedPiece / 8) * yScl, .w = xScl, .h = yScl};
		SDL_SetRenderDrawColor(renderer, 0xed, 0xe9, 0x91, 0x7f);
		SDL_RenderFillRect(renderer, &originalRect);

		MovesArray *moves = Chessboard_computePieceMoves(chessboard, highlightedPiece);

		if (MovesArray_length(moves) >= 1) {

			for (uint32_t i = 0; i < MovesArray_length(moves); ++i) {
				Move move = MovesArray_getMove(moves, i);

				const SDL_Rect moveRect = {.x = (move.dst % 8) * xScl, .y = (7 - move.dst / 8) * yScl, .w = xScl, .h = yScl};
				SDL_SetRenderDrawColor(renderer, 0x8b, 0xe5, 0x8f, 0x7f);
				SDL_RenderFillRect(renderer, &moveRect);
			}
		}
	}

	for (uint8_t pieceType = 0; pieceType < 12; ++pieceType) {
		uint64_t position = chessboard->bitBoard[pieceType];
		uint8_t nBit = 0;
		while (position > 0) {
			if (position & 1) {
				uint8_t row = 7 - nBit / 8;
				uint8_t col = nBit % 8;

				uint32_t spriteX = (5 - pieceType % 6) * 83;
				uint32_t spriteY = pieceType / 6 * 83;

				const SDL_Rect spriteRect = {.x = spriteX, .y = spriteY, .w = 83, .h = 83};
				SDL_Rect dst = {.x = col * xScl, .y = row * yScl, .w = xScl, .h = yScl};

				SDL_RenderCopy(renderer, chessboard->spriteMap, &spriteRect, &dst);
			}
			++nBit;
			position >>= 1;
		}
	}
}

MovesArray *Chessboard_computePieceMoves(Chessboard *chessboard, uint8_t pieceLocation) {
	uint64_t pieceLocationMask = 1ull << pieceLocation;

	uint8_t pieceType = -1;
	for (uint8_t i = 0; i < 12; ++i) {
		if (chessboard->bitBoard[i] & pieceLocationMask) {
			pieceType = i;
			break;
		}
	}

	assert(pieceType < 12);

	uint64_t emptyWhiteMask = 0;
	for (uint8_t i = 0; i < 6; ++i) emptyWhiteMask |= chessboard->bitBoard[i];
	uint64_t emptyBlackMask = 0;
	for (uint8_t i = 6; i < 12; ++i) emptyBlackMask |= chessboard->bitBoard[i];
	uint64_t emptyMask = emptyWhiteMask | emptyBlackMask;

	MovesArray *moves = MovesArray_create();

	switch (pieceType) {
		case 0:
		case 6:;
		uint8_t singlePushLocation, doublePushLocation;
		if (pieceType == 0) {
			singlePushLocation = pieceLocation + 8;
			doublePushLocation = singlePushLocation + 8;
		} else {
			singlePushLocation = pieceLocation - 8;
			doublePushLocation = singlePushLocation - 8;
		}

		uint64_t singlePushLocationMask = 1ull << singlePushLocation;
		uint64_t doublePushLocationMask = 1ull << doublePushLocation;

		if ((singlePushLocationMask & emptyMask) == 0) {
			MovesArray_pushMove(moves, pieceLocation, singlePushLocation);
			if ((doublePushLocationMask & emptyMask) == 0) {
				MovesArray_pushMove(moves, pieceLocation, doublePushLocation);
			}
		}
		break;
		case 1:
		case 7:;
		uint8_t row = pieceLocation / 8;
		uint8_t col = pieceLocation % 8;

		uint64_t verticalPossibleMoves = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			if (i != 7 - row) verticalPossibleMoves |= (1ull << col);
			if (i < 7) verticalPossibleMoves <<= 8;
		}

		uint64_t horizontalPossibleMoves = (((1ull << 8) - 1) ^ (1ull << col)) << (row << 3);
		uint64_t captures = (horizontalPossibleMoves | verticalPossibleMoves) & chessboard->bitBoard[8 - pieceType];
		uint64_t movesMask = (horizontalPossibleMoves & ~emptyMask) | (verticalPossibleMoves & ~emptyMask) | captures;

		uint8_t movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition);
			movesMask >>= 1;
			movePosition++;
		}
		break;
		default:
		fprintf(stderr, "Not implemented '%d'\n", pieceType);
	}

	return moves;
}
