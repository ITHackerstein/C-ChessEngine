#include "Chessboard.h"

Chessboard *createChessboard() {
	Chessboard *chessboard = malloc(sizeof(Chessboard));

	chessboard->bitBoard[0] = 0xff00;               // White Pawn
	chessboard->bitBoard[1] = 0x81;                 // White Rook
	chessboard->bitBoard[2] = 0x42;                 // White Knight
	chessboard->bitBoard[3] = 0x24;                 // White Bishop
	chessboard->bitBoard[4] = 8;                    // White Queen
	chessboard->bitBoard[5] = 0x10;                 // White King
	chessboard->bitBoard[6] = 0xff000000000000;     // Black Pawn
	chessboard->bitBoard[7] = 0x8100000000000000;   // Black Rook
	chessboard->bitBoard[8] = 0x4200000000000000;   // Black Knight
	chessboard->bitBoard[9] = 0x2400000000000000;   // Black Bishop
	chessboard->bitBoard[10] = 0x800000000000000;   // Black Queen
	chessboard->bitBoard[11] = 0x1000000000000000;  // Black King

	chessboard->spriteMap = IMG_Load("res/Chess_Pieces.png");

	return chessboard;
}

void drawChessboard(Chessboard *chessboard, SDL_Surface *surf) {
	const uint32_t xScl = surf->w / 8;
	const uint32_t yScl = surf->h / 8;

	for (uint32_t j = 0; j < 8; ++j) {
		for (uint32_t i = 0; i < 8; ++i) {
			const SDL_Rect tileRect = {.x = xScl * i, .y = yScl * j, .w = xScl, .h = yScl};
			if ((i + j) % 2 == 0)
				SDL_FillRect(surf, &tileRect, SDL_MapRGB(surf->format, 0xe8, 0xeb, 0xef));
			else
				SDL_FillRect(surf, &tileRect, SDL_MapRGB(surf->format, 0x7d, 0x87, 0x96));
		}
	}

	for (uint32_t pieceType = 0; pieceType < 12; ++pieceType) {
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

				SDL_BlitScaled(chessboard->spriteMap, &spriteRect, surf, &dst);
			}
			++nBit;
			position >>= 1;
		}
	}
}
