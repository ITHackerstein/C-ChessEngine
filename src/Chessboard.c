#include "Chessboard.h"

static uint8_t LS1B(uint64_t n) {
	if (n == 0) return 64;

	uint8_t ls1b = 0;
	while (n && (n & 1) == 0) {
		n >>= 1;
		ls1b++;
	}
	return ls1b;
}

static uint8_t MS1B(uint64_t n) {
	if (n == 0) return 64;

	uint8_t ms1b = 0;
	while (n) {
		n >>= 1;
		if (n == 0) return ms1b;
		ms1b++;
	}
}

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

	chessboard->knightMoves[0] = 0x20400ull;
	chessboard->knightMoves[1] = 0x50800ull;
	chessboard->knightMoves[2] = 0xa1100ull;
	chessboard->knightMoves[3] = 0x142200ull;
	chessboard->knightMoves[4] = 0x284400ull;
	chessboard->knightMoves[5] = 0x508800ull;
	chessboard->knightMoves[6] = 0xa01000ull;
	chessboard->knightMoves[7] = 0x402000ull;
	chessboard->knightMoves[8] = 0x2040004ull;
	chessboard->knightMoves[9] = 0x5080008ull;
	chessboard->knightMoves[10] = 0xa110011ull;
	chessboard->knightMoves[11] = 0x14220022ull;
	chessboard->knightMoves[12] = 0x28440044ull;
	chessboard->knightMoves[13] = 0x50880088ull;
	chessboard->knightMoves[14] = 0xa0100010ull;
	chessboard->knightMoves[15] = 0x40200020ull;
	chessboard->knightMoves[16] = 0x204000402ull;
	chessboard->knightMoves[17] = 0x508000805ull;
	chessboard->knightMoves[18] = 0xa1100110aull;
	chessboard->knightMoves[19] = 0x1422002214ull;
	chessboard->knightMoves[20] = 0x2844004428ull;
	chessboard->knightMoves[21] = 0x5088008850ull;
	chessboard->knightMoves[22] = 0xa0100010a0ull;
	chessboard->knightMoves[23] = 0x4020002040ull;
	chessboard->knightMoves[24] = 0x20400040200ull;
	chessboard->knightMoves[25] = 0x50800080500ull;
	chessboard->knightMoves[26] = 0xa1100110a00ull;
	chessboard->knightMoves[27] = 0x142200221400ull;
	chessboard->knightMoves[28] = 0x284400442800ull;
	chessboard->knightMoves[29] = 0x508800885000ull;
	chessboard->knightMoves[30] = 0xa0100010a000ull;
	chessboard->knightMoves[31] = 0x402000204000ull;
	chessboard->knightMoves[32] = 0x2040004020000ull;
	chessboard->knightMoves[33] = 0x5080008050000ull;
	chessboard->knightMoves[34] = 0xa1100110a0000ull;
	chessboard->knightMoves[35] = 0x14220022140000ull;
	chessboard->knightMoves[36] = 0x28440044280000ull;
	chessboard->knightMoves[37] = 0x50880088500000ull;
	chessboard->knightMoves[38] = 0xa0100010a00000ull;
	chessboard->knightMoves[39] = 0x40200020400000ull;
	chessboard->knightMoves[40] = 0x204000402000000ull;
	chessboard->knightMoves[41] = 0x508000805000000ull;
	chessboard->knightMoves[42] = 0xa1100110a000000ull;
	chessboard->knightMoves[43] = 0x1422002214000000ull;
	chessboard->knightMoves[44] = 0x2844004428000000ull;
	chessboard->knightMoves[45] = 0x5088008850000000ull;
	chessboard->knightMoves[46] = 0xa0100010a0000000ull;
	chessboard->knightMoves[47] = 0x4020002040000000ull;
	chessboard->knightMoves[48] = 0x400040200000000ull;
	chessboard->knightMoves[49] = 0x800080500000000ull;
	chessboard->knightMoves[50] = 0x1100110a00000000ull;
	chessboard->knightMoves[51] = 0x2200221400000000ull;
	chessboard->knightMoves[52] = 0x4400442800000000ull;
	chessboard->knightMoves[53] = 0x8800885000000000ull;
	chessboard->knightMoves[54] = 0x100010a000000000ull;
	chessboard->knightMoves[55] = 0x2000204000000000ull;
	chessboard->knightMoves[56] = 0x4020000000000ull;
	chessboard->knightMoves[57] = 0x8050000000000ull;
	chessboard->knightMoves[58] = 0x110a0000000000ull;
	chessboard->knightMoves[59] = 0x22140000000000ull;
	chessboard->knightMoves[60] = 0x44280000000000ull;
	chessboard->knightMoves[61] = 0x88500000000000ull;
	chessboard->knightMoves[62] = 0x10a00000000000ull;
	chessboard->knightMoves[63] = 0x20400000000000ull;

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

	if (pieceType == 0 || pieceType == 6) {
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
	} else if (pieceType == 1 || pieceType == 7) {
		uint8_t row = pieceLocation / 8;
		uint8_t col = pieceLocation % 8;

		uint64_t verticalPossibleMoves = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			if (i != 7 - row) verticalPossibleMoves |= (1ull << col);
			if (i < 7) verticalPossibleMoves <<= 8;
		}

		uint64_t horizontalPossibleMoves = (((1ull << 8) - 1) ^ (1ull << col)) << (row << 3);


		uint8_t minVert = MS1B(verticalPossibleMoves & ((1ull << (row << 3)) - 1) & emptyMask);

		if (minVert == 64) minVert = col;
		else minVert += 8;

		uint8_t maxVert = LS1B(verticalPossibleMoves & (((1ull << ((7 - row) << 3)) - 1) << ((row + 1) << 3)) & emptyMask);

		if (maxVert == 64) maxVert = 56 + col;
		else maxVert -= 8;

		if (maxVert - minVert + 1 != 64)
			verticalPossibleMoves &= ((1ull << (maxVert - minVert + 1)) - 1) << minVert;

		uint8_t minHoriz = MS1B(horizontalPossibleMoves & (((1ull << col) - 1) << (row << 3)) & emptyMask);

		if (minHoriz == 64) minHoriz = row * 8;
		else minHoriz++;

		uint8_t maxHoriz = LS1B(horizontalPossibleMoves & (((1ull << (7 - col)) - 1) << ((row << 3) + col + 1)) & emptyMask);

		if (maxHoriz == 64) maxHoriz = row * 8 + 7;
		else maxHoriz--;

		if (maxHoriz - minHoriz + 1 != 64)
			horizontalPossibleMoves &= ((1ull << (maxHoriz - minHoriz + 1)) - 1) << minHoriz;

		uint64_t movesMask = verticalPossibleMoves | horizontalPossibleMoves;

		uint8_t movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition);
			movesMask >>= 1;
			movePosition++;
		}
	} else if (pieceType == 2 || pieceType == 8) {
		uint8_t movePosition = 0;
		uint64_t pieceMoves = chessboard->knightMoves[pieceLocation] & ~emptyMask;
		while (pieceMoves) {
			if (pieceMoves & 1) MovesArray_pushMove(moves, pieceLocation, movePosition);
			pieceMoves >>= 1;
			movePosition++;
		}
	} else if (pieceType == 3 || pieceType == 9) {
		uint64_t row = pieceLocation / 8;
		uint64_t col = pieceLocation % 8;

		int8_t shiftAmt;

		uint64_t leftDiagonalMoves = 0;

		shiftAmt = 7 - row - col;
		if (shiftAmt < 0) shiftAmt *= -1;

		for (uint8_t j = 0; j < 8; ++j) {
			if (7 - row > col)
				leftDiagonalMoves |= ((1ull << j) >> shiftAmt) & 0xff;
			else
				leftDiagonalMoves |= ((1ull << j) << shiftAmt) & 0xff;

			if (j < 7) leftDiagonalMoves <<= 8;
		}
		leftDiagonalMoves ^= 1ull << pieceLocation;

		uint64_t rightDiagonalMoves = 0;

		shiftAmt = row - col;
		if (shiftAmt < 0) shiftAmt *= -1;

		for (uint8_t j = 0; j < 8; ++j) {
			if (row > col)
				rightDiagonalMoves |= ((1ull << (7 - j)) >> shiftAmt) & 0xff;
			else
				rightDiagonalMoves |= ((1ull << (7 - j)) << shiftAmt) & 0xff;

			if (j < 7) rightDiagonalMoves <<= 8;
		}
		rightDiagonalMoves ^= 1ull << pieceLocation;

		uint64_t movesMask = leftDiagonalMoves | rightDiagonalMoves;

		uint64_t leftMask = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			leftMask |= ((1ull << (7 - col)) - 1) << (col + 1);
			if (i < 7) leftMask <<= 8;
		}

		uint64_t rightMask = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			rightMask |= (1ull << col) - 1;
			if (i < 7) rightMask <<= 8;
		}

		uint64_t topMask = ((1ull << ((7 - row) << 3)) - 1) << ((row + 1) << 3);
		uint64_t bottomMask = (1ull << (row << 3)) - 1;

		uint64_t botRightMovesMask = movesMask & rightMask & bottomMask;
		uint64_t topLeftMovesMask = movesMask & leftMask & topMask;
		uint64_t botLeftMovesMask = movesMask & leftMask & bottomMask;
		uint64_t topRightMovesMask = movesMask & rightMask & topMask;

		uint8_t leftMin = MS1B(botLeftMovesMask & emptyMask);
		uint8_t leftMax = LS1B(topRightMovesMask & emptyMask);
		uint8_t rightMin = MS1B(botRightMovesMask & emptyMask);
		uint8_t rightMax = LS1B(topLeftMovesMask & emptyMask);

		if (leftMin == 64) {
			if (botLeftMovesMask == 0)
				leftMin = pieceLocation;

			if ((botLeftMovesMask & emptyMask) == 0)
				leftMin = LS1B(leftDiagonalMoves);
		} else {
			leftMin += 7;
		}

		if (leftMax == 64) {
			if (topRightMovesMask == 0)
				leftMax = pieceLocation;

			if ((topRightMovesMask & emptyMask) == 0) {
				leftMax = MS1B(leftDiagonalMoves);
			}
		} else {
			leftMax -= 7;
		}

		if (rightMin == 64) {
			if (botRightMovesMask == 0)
				rightMin = pieceLocation;

			if ((botRightMovesMask & emptyMask) == 0)
				rightMin = LS1B(rightDiagonalMoves);
		} else {
			rightMin += 9;
		}

		if (rightMax == 64) {
			if (topLeftMovesMask == 0)
				rightMax = pieceLocation;

			if ((topLeftMovesMask & emptyMask) == 0)
				rightMax = MS1B(rightDiagonalMoves);
		} else {
			rightMax -= 9;
		}

		movesMask = 0;
		for (uint8_t pos = leftMin; pos <= leftMax; pos += 7) {
			if (pos != pieceLocation)
				movesMask |= (1ull << pos);
		}

		for (uint8_t pos = rightMin; pos <= rightMax; pos += 9) {
			if (pos != pieceLocation)
				movesMask |= (1ull << pos);
		}

		uint8_t movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition);
			movesMask >>= 1;
			movePosition++;
		}
	} else if (pieceType == 4 || pieceType == 10) {
		uint8_t row = pieceLocation / 8;
		uint8_t col = pieceLocation % 8;

		uint64_t movesMask, movePosition;

		uint64_t verticalPossibleMoves = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			if (i != 7 - row) verticalPossibleMoves |= (1ull << col);
			if (i < 7) verticalPossibleMoves <<= 8;
		}

		uint64_t horizontalPossibleMoves = (((1ull << 8) - 1) ^ (1ull << col)) << (row << 3);


		uint8_t minVert = MS1B(verticalPossibleMoves & ((1ull << (row << 3)) - 1) & emptyMask);

		if (minVert == 64) minVert = col;
		else minVert += 8;

		uint8_t maxVert = LS1B(verticalPossibleMoves & (((1ull << ((7 - row) << 3)) - 1) << ((row + 1) << 3)) & emptyMask);

		if (maxVert == 64) maxVert = 56 + col;
		else maxVert -= 8;

		if (maxVert - minVert + 1 != 64)
			verticalPossibleMoves &= ((1ull << (maxVert - minVert + 1)) - 1) << minVert;

		uint8_t minHoriz = MS1B(horizontalPossibleMoves & (((1ull << col) - 1) << (row << 3)) & emptyMask);

		if (minHoriz == 64) minHoriz = row * 8;
		else minHoriz++;

		uint8_t maxHoriz = LS1B(horizontalPossibleMoves & (((1ull << (7 - col)) - 1) << ((row << 3) + col + 1)) & emptyMask);

		if (maxHoriz == 64) maxHoriz = row * 8 + 7;
		else maxHoriz--;

		if (maxHoriz - minHoriz + 1 != 64)
			horizontalPossibleMoves &= ((1ull << (maxHoriz - minHoriz + 1)) - 1) << minHoriz;

		movesMask = verticalPossibleMoves | horizontalPossibleMoves;

		movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition);
			movesMask >>= 1;
			movePosition++;
		}

		int8_t shiftAmt;

		uint64_t leftDiagonalMoves = 0;

		shiftAmt = 7 - row - col;
		if (shiftAmt < 0) shiftAmt *= -1;

		for (uint8_t j = 0; j < 8; ++j) {
			if (7 - row > col)
				leftDiagonalMoves |= ((1ull << j) >> shiftAmt) & 0xff;
			else
				leftDiagonalMoves |= ((1ull << j) << shiftAmt) & 0xff;

			if (j < 7) leftDiagonalMoves <<= 8;
		}
		leftDiagonalMoves ^= 1ull << pieceLocation;

		uint64_t rightDiagonalMoves = 0;

		shiftAmt = row - col;
		if (shiftAmt < 0) shiftAmt *= -1;

		for (uint8_t j = 0; j < 8; ++j) {
			if (row > col)
				rightDiagonalMoves |= ((1ull << (7 - j)) >> shiftAmt) & 0xff;
			else
				rightDiagonalMoves |= ((1ull << (7 - j)) << shiftAmt) & 0xff;

			if (j < 7) rightDiagonalMoves <<= 8;
		}
		rightDiagonalMoves ^= 1ull << pieceLocation;

		movesMask = leftDiagonalMoves | rightDiagonalMoves;

		uint64_t leftMask = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			leftMask |= ((1ull << (7 - col)) - 1) << (col + 1);
			if (i < 7) leftMask <<= 8;
		}

		uint64_t rightMask = 0;
		for (uint8_t i = 0; i < 8; ++i) {
			rightMask |= (1ull << col) - 1;
			if (i < 7) rightMask <<= 8;
		}

		uint64_t topMask = ((1ull << ((7 - row) << 3)) - 1) << ((row + 1) << 3);
		uint64_t bottomMask = (1ull << (row << 3)) - 1;

		uint64_t botRightMovesMask = movesMask & rightMask & bottomMask;
		uint64_t topLeftMovesMask = movesMask & leftMask & topMask;
		uint64_t botLeftMovesMask = movesMask & leftMask & bottomMask;
		uint64_t topRightMovesMask = movesMask & rightMask & topMask;

		uint8_t leftMin = MS1B(botLeftMovesMask & emptyMask);
		uint8_t leftMax = LS1B(topRightMovesMask & emptyMask);
		uint8_t rightMin = MS1B(botRightMovesMask & emptyMask);
		uint8_t rightMax = LS1B(topLeftMovesMask & emptyMask);

		if (leftMin == 64) {
			if (botLeftMovesMask == 0)
				leftMin = pieceLocation;

			if ((botLeftMovesMask & emptyMask) == 0)
				leftMin = LS1B(leftDiagonalMoves);
		} else {
			leftMin += 7;
		}

		if (leftMax == 64) {
			if (topRightMovesMask == 0)
				leftMax = pieceLocation;

			if ((topRightMovesMask & emptyMask) == 0) {
				leftMax = MS1B(leftDiagonalMoves);
			}
		} else {
			leftMax -= 7;
		}

		if (rightMin == 64) {
			if (botRightMovesMask == 0)
				rightMin = pieceLocation;

			if ((botRightMovesMask & emptyMask) == 0)
				rightMin = LS1B(rightDiagonalMoves);
		} else {
			rightMin += 9;
		}

		if (rightMax == 64) {
			if (topLeftMovesMask == 0)
				rightMax = pieceLocation;

			if ((topLeftMovesMask & emptyMask) == 0)
				rightMax = MS1B(rightDiagonalMoves);
		} else {
			rightMax -= 9;
		}

		movesMask = 0;
		for (uint8_t pos = leftMin; pos <= leftMax; pos += 7) {
			if (pos != pieceLocation)
				movesMask |= (1ull << pos);
		}

		for (uint8_t pos = rightMin; pos <= rightMax; pos += 9) {
			if (pos != pieceLocation)
				movesMask |= (1ull << pos);
		}

		movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition);
			movesMask >>= 1;
			movePosition++;
		}
	} else if (pieceType == 5 || pieceType == 11) {
		uint8_t row = pieceLocation / 8;
		uint8_t col = pieceLocation % 8;

		for (int8_t j = -1; j < 2; ++j) {
			for (int8_t i = -1; i < 2; ++i) {
				if (j == 0 && i == 0) continue;
				if (row + j < 0 || row + j >= 8 || col + i < 0 || col + i >= 8) continue;

				uint8_t movePosition = (row + j) * 8 + col + i;
				if (((1ull << movePosition) & emptyMask) == 0)
					MovesArray_pushMove(moves, pieceLocation, movePosition);
			}
		}
	} else {
		fprintf(stderr, "Not implemented '%d'\n", pieceType);
	}

	return moves;
}
