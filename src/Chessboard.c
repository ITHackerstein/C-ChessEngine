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
		MovesArray *moves = Chessboard_computePieceMoves(chessboard, highlightedPiece);

		if (moves != NULL) {
			const SDL_Rect originalRect = {.x = (highlightedPiece % 8) * xScl, .y = (7 - highlightedPiece / 8) * yScl, .w = xScl, .h = yScl};
			SDL_SetRenderDrawColor(renderer, 0xed, 0xe9, 0x91, 0x7f);
			SDL_RenderFillRect(renderer, &originalRect);

			if (MovesArray_length(moves) >= 1) {

				for (uint32_t i = 0; i < MovesArray_length(moves); ++i) {
					Move move = MovesArray_getMove(moves, i);

					const SDL_Rect moveRect = {.x = (move.dst % 8) * xScl, .y = (7 - move.dst / 8) * yScl, .w = xScl, .h = yScl};
					if (!move.isCapture)
						SDL_SetRenderDrawColor(renderer, 0x8b, 0xe5, 0x8f, 0x7f);
					else
						SDL_SetRenderDrawColor(renderer, 0xdb, 0x53, 0x56, 0x7f);
					SDL_RenderFillRect(renderer, &moveRect);
				}
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

	if (pieceType >= 12) return NULL;

	MovesArray *moves = MovesArray_create();

	uint64_t emptyWhiteMask = 0;
	for (uint8_t i = 0; i < 6; ++i) emptyWhiteMask |= chessboard->bitBoard[i];
	uint64_t emptyBlackMask = 0;
	for (uint8_t i = 6; i < 12; ++i) emptyBlackMask |= chessboard->bitBoard[i];
	uint64_t emptyMask = emptyWhiteMask | emptyBlackMask;

	uint64_t emptyEnemyMask = pieceType < 6 ? emptyBlackMask : emptyWhiteMask;

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
			MovesArray_pushMove(moves, pieceLocation, singlePushLocation, false);
			if ((doublePushLocationMask & emptyMask) == 0) {
				MovesArray_pushMove(moves, pieceLocation, doublePushLocation, false);
			}
		}

		uint8_t leftCaptureLocation, rightCaptureLocation;
		if (pieceType == 0) {
			leftCaptureLocation = pieceLocation + 7;
			rightCaptureLocation = pieceLocation + 9;
		} else {
			leftCaptureLocation = pieceLocation - 7;
			rightCaptureLocation = pieceLocation - 9;
		}

		if (leftCaptureLocation / 8 != pieceLocation / 8) {
			if (((1ull << leftCaptureLocation) & emptyEnemyMask) != 0) MovesArray_pushMove(moves, pieceLocation, leftCaptureLocation, true);
		}

		if (rightCaptureLocation / 8 != pieceLocation / 8) {
			if (((1ull << rightCaptureLocation) & emptyEnemyMask) != 0) MovesArray_pushMove(moves, pieceLocation, rightCaptureLocation, true);
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

		uint8_t movePosition;

		movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, false);
			movesMask >>= 1;
			movePosition++;
		}

		uint64_t capturesMask = (((1ull << minVert) >> 8) | ((1ull << maxVert) << 8) | ((1ull << minHoriz) >> 1) | ((1ull << maxHoriz) << 1)) & emptyEnemyMask;

		movePosition = 0;
		while (capturesMask) {
			if (capturesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, true);
			capturesMask >>= 1;
			movePosition++;
		}

	} else if (pieceType == 2 || pieceType == 8) {
		uint64_t row = pieceLocation / 8;
		uint64_t col = pieceLocation % 8;

		int8_t X[8] = {1, 2, 2, 1, -1, -2, -2, -1};
		int8_t Y[8] = {2, 1, -1, -2, -2, -1, 1, 2};

		for (uint8_t idx = 0; idx < 8; ++idx) {
			int8_t j = Y[idx];
			int8_t i = X[idx];

			if (row + j < 0 || row + j >= 8 || col + i < 0 || col + i >= 8) continue;

			uint8_t movePosition = (row + j) * 8 + col + i;
			uint64_t movePositionMask = 1ull << movePosition;

			if ((movePositionMask & emptyMask) == 0)
				MovesArray_pushMove(moves, pieceLocation, movePosition, false);

			if ((movePositionMask & emptyEnemyMask) != 0)
				MovesArray_pushMove(moves, pieceLocation, movePosition, true);
		}
	} else if (pieceType == 3 || pieceType == 9) {
		uint8_t row = pieceLocation / 8;
		uint8_t col = pieceLocation % 8;

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

		uint64_t capturesMask = 0;

		if (leftMin == 64) {
			if (botLeftMovesMask == 0)
				leftMin = pieceLocation;

			if ((botLeftMovesMask & emptyMask) == 0)
				leftMin = LS1B(leftDiagonalMoves);
		} else {
			capturesMask |= (1ull << leftMin) & emptyEnemyMask;
			leftMin += 7;
		}

		if (leftMax == 64) {
			if (topRightMovesMask == 0)
				leftMax = pieceLocation;

			if ((topRightMovesMask & emptyMask) == 0) {
				leftMax = MS1B(leftDiagonalMoves);
			}
		} else {
			capturesMask |= (1ull << leftMax) & emptyEnemyMask;
			leftMax -= 7;
		}

		if (rightMin == 64) {
			if (botRightMovesMask == 0)
				rightMin = pieceLocation;

			if ((botRightMovesMask & emptyMask) == 0)
				rightMin = LS1B(rightDiagonalMoves);
		} else {
			capturesMask |= (1ull << rightMin) & emptyEnemyMask;
			rightMin += 9;
		}

		if (rightMax == 64) {
			if (topLeftMovesMask == 0)
				rightMax = pieceLocation;

			if ((topLeftMovesMask & emptyMask) == 0)
				rightMax = MS1B(rightDiagonalMoves);
		} else {
			capturesMask |= (1ull << rightMax) & emptyEnemyMask;
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

		uint8_t movePosition;

		movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, false);
			movesMask >>= 1;
			movePosition++;
		}

		movePosition = 0;
		while (capturesMask) {
			if (capturesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, true);
			capturesMask >>= 1;
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

		uint64_t capturesMask;

		capturesMask = (((1ull << minVert) >> 8) | ((1ull << maxVert) << 8) | ((1ull << minHoriz) >> 1) | ((1ull << maxHoriz) << 1)) & emptyEnemyMask;

		movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, false);
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
			capturesMask |= (1ull << leftMin) & emptyEnemyMask;
			leftMin += 7;
		}

		if (leftMax == 64) {
			if (topRightMovesMask == 0)
				leftMax = pieceLocation;

			if ((topRightMovesMask & emptyMask) == 0) {
				leftMax = MS1B(leftDiagonalMoves);
			}
		} else {
			capturesMask |= (1ull << leftMax) & emptyEnemyMask;
			leftMax -= 7;
		}

		if (rightMin == 64) {
			if (botRightMovesMask == 0)
				rightMin = pieceLocation;

			if ((botRightMovesMask & emptyMask) == 0)
				rightMin = LS1B(rightDiagonalMoves);
		} else {
			capturesMask |= (1ull << rightMin) & emptyEnemyMask;
			rightMin += 9;
		}

		if (rightMax == 64) {
			if (topLeftMovesMask == 0)
				rightMax = pieceLocation;

			if ((topLeftMovesMask & emptyMask) == 0)
				rightMax = MS1B(rightDiagonalMoves);
		} else {
			capturesMask |= (1ull << rightMax) & emptyEnemyMask;
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
			if (movesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, false);
			movesMask >>= 1;
			movePosition++;
		}

		movePosition = 0;
		while (capturesMask) {
			if (capturesMask & 1) MovesArray_pushMove(moves, pieceLocation, movePosition, true);
			capturesMask >>= 1;
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
				uint64_t movePositionMask = (1ull << movePosition);

				if ((movePositionMask & emptyMask) == 0)
					MovesArray_pushMove(moves, pieceLocation, movePosition, false);

				if ((movePositionMask & emptyEnemyMask) != 0)
					MovesArray_pushMove(moves, pieceLocation, movePosition, true);
			}
		}
	} else {
		fprintf(stderr, "Not implemented '%d'\n", pieceType);
	}

	return moves;
}
