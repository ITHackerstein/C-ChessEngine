#include "Chessboard.h"

Chessboard *Chessboard_create(SDL_Renderer *renderer) {
	Chessboard *chessboard = malloc(sizeof(Chessboard));

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
	int w, h;
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
		MovesArray *moves = Chessboard_computePieceMoves(chessboard, highlightedPiece);

		if (MovesArray_length(moves) >= 1) {
			const SDL_Rect originalRect = {.x = (highlightedPiece % 8) * xScl, .y = (7 - highlightedPiece / 8) * yScl, .w = xScl, .h = yScl};
			SDL_SetRenderDrawColor(renderer, 0xed, 0xe9, 0x91, 0x7f);
			SDL_RenderFillRect(renderer, &originalRect);

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

	uint64_t emptyMask = 0;
	for (uint8_t i = 0; i < 12; ++i) emptyMask |= chessboard->bitBoard[i];

	MovesArray *moves = MovesArray_create();

	switch (pieceType) {
		case 0:;
		uint8_t singlePushLocationW = pieceLocation + 8;
		uint8_t doublePushLocationW = singlePushLocationW + 8;

		uint64_t singlePushLocationMaskW = 1ull << singlePushLocationW;
		uint64_t doublePushLocationMaskW = 1ull << doublePushLocationW;

		if ((singlePushLocationMaskW & emptyMask) == 0) {
			MovesArray_pushMove(moves, pieceLocation, singlePushLocationW);
			if ((doublePushLocationMaskW & emptyMask) == 0) {
				MovesArray_pushMove(moves, pieceLocation, doublePushLocationW);
			}
		}

		break;
		case 6:;
		uint8_t singlePushLocationB = pieceLocation - 8;
		uint8_t doublePushLocationB = singlePushLocationB - 8;

		uint64_t singlePushLocationMaskB = 1ull << singlePushLocationB;
		uint64_t doublePushLocationMaskB = 1ull << doublePushLocationB;

		if ((singlePushLocationMaskB & emptyMask) == 0) {
			MovesArray_pushMove(moves, pieceLocation, singlePushLocationB);
			if ((doublePushLocationMaskB & emptyMask) == 0) {
				MovesArray_pushMove(moves, pieceLocation, doublePushLocationB);
			}
		}

		break;
		default:
		fprintf(stderr, "Not implemented '%d'\n", pieceType);
	}

	return moves;
}
