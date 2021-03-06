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

void Chessboard_draw(Chessboard *chessboard, SDL_Renderer *renderer, uint8_t highlightedPiece, MovesArray *highlightedPieceMoves) {
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

	if (highlightedPieceMoves != NULL) {
		const SDL_Rect originalRect = {.x = (highlightedPiece % 8) * xScl, .y = (7 - highlightedPiece / 8) * yScl, .w = xScl, .h = yScl};
		SDL_SetRenderDrawColor(renderer, 0xed, 0xe9, 0x91, 0x7f);
		SDL_RenderFillRect(renderer, &originalRect);

		if (MovesArray_length(highlightedPieceMoves) >= 1) {
			for (uint32_t i = 0; i < MovesArray_length(highlightedPieceMoves); ++i) {
				Move move = MovesArray_getMove(highlightedPieceMoves, i);

				const SDL_Rect moveRect = {.x = (move.dst % 8) * xScl, .y = (7 - move.dst / 8) * yScl, .w = xScl, .h = yScl};
				if (!move.isCapture)
					SDL_SetRenderDrawColor(renderer, 0x8b, 0xe5, 0x8f, 0x7f);
				else
					SDL_SetRenderDrawColor(renderer, 0xdb, 0x53, 0x56, 0x7f);
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

MovesArray *Chessboard_computePieceMoves(Chessboard *chessboard, uint8_t pieceLocation, bool checkCastling, bool onlyLegalMoves) {
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
		uint64_t initialRankMask = ((1ull << 8) - 1) << (pieceType == 0 ? 8 : 48);

		if ((singlePushLocationMask & emptyMask) == 0) {
			Move singlePushMove = {.srcPieceType = pieceType, .src = pieceLocation, .dst = singlePushLocation, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
			MovesArray_pushMove(moves, singlePushMove);
			if ((doublePushLocationMask & emptyMask) == 0 && (pieceLocationMask & initialRankMask) != 0) {
				Move doublePushMove = {.srcPieceType = pieceType, .src = pieceLocation, .dst = doublePushLocation, .isCapture = false, .isEnPassant = true, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, doublePushMove);
			}
		}

		uint8_t leftCaptureLocation, rightCaptureLocation;

		if (pieceType == 0) {
			leftCaptureLocation = pieceLocation + 7;
			rightCaptureLocation = pieceLocation + 9;
		} else {
			rightCaptureLocation = pieceLocation - 7;
			leftCaptureLocation = pieceLocation - 9;
		}

		uint64_t leftCaptureLocationMask = 1ull << leftCaptureLocation;
		uint64_t rightCaptureLocationMask = 1ull << rightCaptureLocation;

		if (leftCaptureLocation / 8 != pieceLocation / 8) {
			if ((leftCaptureLocationMask & emptyEnemyMask) != 0 || (chessboard->lastMove.isEnPassant && chessboard->lastMove.dst == pieceLocation - 1)) {
				Move leftCaptureMove = {.srcPieceType = pieceType, .src = pieceLocation, .dst = leftCaptureLocation, .isCapture = true, .isEnPassant = chessboard->lastMove.isEnPassant, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, leftCaptureMove);
			}
		}

		if (rightCaptureLocation / 8 != pieceLocation / 8) {
			if ((rightCaptureLocationMask & emptyEnemyMask) != 0 || (chessboard->lastMove.isEnPassant && chessboard->lastMove.dst == pieceLocation + 1)) {
				Move rightCaptureMove = {.srcPieceType = pieceType, .src = pieceLocation, .dst = rightCaptureLocation, .isCapture = true, .isEnPassant = chessboard->lastMove.isEnPassant, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, rightCaptureMove);
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

		uint8_t movePosition;

		movePosition = 0;
		while (movesMask) {
			if (movesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
			movesMask >>= 1;
			movePosition++;
		}

		uint64_t capturesMask = (((1ull << minVert) >> 8) | ((1ull << maxVert) << 8) | ((1ull << minHoriz) >> 1) | ((1ull << maxHoriz) << 1)) & emptyEnemyMask;

		movePosition = 0;
		while (capturesMask) {
			if (capturesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = true, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
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

			if ((movePositionMask & emptyMask) == 0) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}

			if ((movePositionMask & emptyEnemyMask) != 0) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = true, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
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
			if (movesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
			movesMask >>= 1;
			movePosition++;
		}

		movePosition = 0;
		while (capturesMask) {
			if (capturesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = true, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
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
			if (movesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
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
			if (movesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
			movesMask >>= 1;
			movePosition++;
		}

		movePosition = 0;
		while (capturesMask) {
			if (capturesMask & 1) {
				Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = true, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
				MovesArray_pushMove(moves, move);
			}
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

				if ((movePositionMask & emptyMask) == 0) {
					Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = false, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
					MovesArray_pushMove(moves, move);
				}

				if ((movePositionMask & emptyEnemyMask) != 0) {
					Move move = {.srcPieceType = pieceType, .src = pieceLocation, .dst = movePosition, .isCapture = true, .isEnPassant = false, .isCastling = false, .isLeftCastling = false};
					MovesArray_pushMove(moves, move);
				}
			}
		}

		if (checkCastling && !Chessboard_kingInCheck(chessboard, pieceType)) {
			uint64_t rooksLocationMask, kingInitialLocationMask, leftRookInitialLocationMask, rightRookInitialLocationMask, leftEmptySquaresMask, rightEmptySquaresMask;
			uint8_t leftMoveSrc, rightMoveSrc, leftMoveDst, rightMoveDst, attacker;

			if (pieceType == 5) {
				rooksLocationMask = chessboard->bitBoard[1];
				kingInitialLocationMask = 0x10ull;
				leftRookInitialLocationMask = 1ull;
				rightRookInitialLocationMask = 0x80ull;
				leftEmptySquaresMask = 0xeull;
				rightEmptySquaresMask = 0x60ull;
				leftMoveSrc = 0;
				rightMoveSrc = 7;
				leftMoveDst = 2;
				rightMoveDst = 6;
				attacker = 1;
			} else {
				rooksLocationMask = chessboard->bitBoard[7];
				kingInitialLocationMask = 0x1000000000000000ull;
				leftRookInitialLocationMask = 0x100000000000000ull;
				rightRookInitialLocationMask = 0x8000000000000000ull;
				leftEmptySquaresMask = 0xe00000000000000ull;
				rightEmptySquaresMask = 0x6000000000000000ull;
				leftMoveSrc = 56;
				rightMoveSrc = 63;
				leftMoveDst = 58;
				rightMoveDst = 62;
				attacker = 0;
			}

			if ((pieceLocationMask & kingInitialLocationMask) && (rooksLocationMask & leftRookInitialLocationMask)) {
				if ((emptyMask & leftEmptySquaresMask) == 0) {
					if (!Chessboard_squareAttacked(chessboard, attacker, pieceLocation - 1) && !Chessboard_squareAttacked(chessboard, attacker, pieceLocation - 2)) {
						Move move = {.srcPieceType = pieceType, .src = leftMoveSrc, .dst = leftMoveDst, .isCapture = false, .isEnPassant = false, .isCastling = true, .isLeftCastling = true};
						MovesArray_pushMove(moves, move);
					}
				}
			}

			if ((pieceLocationMask & kingInitialLocationMask) && (rooksLocationMask & rightRookInitialLocationMask)) {
				if ((emptyMask & rightEmptySquaresMask) == 0) {
					if (!Chessboard_squareAttacked(chessboard, attacker, pieceLocation + 1) && !Chessboard_squareAttacked(chessboard, attacker, pieceLocation + 2)) {
						Move move = {.srcPieceType = pieceType, .src = rightMoveSrc, .dst = rightMoveDst, .isCapture = false, .isEnPassant = false, .isCastling = true, .isLeftCastling = false};
						MovesArray_pushMove(moves, move);
					}
				}
			}
		}
	}

	if (onlyLegalMoves) {
		MovesArray *finalMoves = MovesArray_create();
		uint8_t kingType = pieceType < 6 ? 5 : 11;

		for (uint8_t i = 0; i < MovesArray_length(moves); ++i) {
			Move move = MovesArray_getMove(moves, i);

			if (Chessboard_isMoveLegal(chessboard, move))
				MovesArray_pushMove(finalMoves, move);
		}

		MovesArray_destroy(moves);

		return finalMoves;
	}

	return moves;
}

MovesArray *Chessboard_computeAllMoves(Chessboard *chessboard, uint8_t side, bool checkCastling, bool onlyLegalMoves) {
	uint64_t movingSideMask = 0;
	for (uint8_t i = side * 6; i < side * 6 + 6; ++i) movingSideMask |= chessboard->bitBoard[i];

	MovesArray *allMoves = MovesArray_create();

	uint8_t pieceLocation = 0;
	while (movingSideMask) {
		if (movingSideMask & 1) {
			MovesArray *pieceMoves = Chessboard_computePieceMoves(chessboard, pieceLocation, checkCastling, onlyLegalMoves);

			if (pieceMoves != NULL) {
				for (uint8_t i = 0; i < MovesArray_length(pieceMoves); ++i)
					MovesArray_pushMove(allMoves, MovesArray_getMove(pieceMoves, i));

				MovesArray_destroy(pieceMoves);
			}
		}
		pieceLocation++;
		movingSideMask >>= 1;
	}

	return allMoves;
}

bool Chessboard_isHighlightable(Chessboard *chessboard, uint8_t pieceLocation, uint8_t turn) {
	uint8_t colorOffset = turn ? 6 : 0;
	uint64_t pieceLocationMask = 1ull << pieceLocation;

	for (uint8_t i = 0; i < 6; ++i) {
		if (chessboard->bitBoard[colorOffset + i] & pieceLocationMask)
			return true;
	}
	return false;
}

void Chessboard_applyMove(Chessboard *chessboard, Move move) {
	if (move.isCastling) {
		chessboard->bitBoard[move.srcPieceType] = 1ull << move.dst;

		uint8_t rookType = move.srcPieceType == 5 ? 1 : 7;

		if (move.isLeftCastling)
			chessboard->bitBoard[rookType] ^= (1ull << move.src) | (1ull << (move.dst + 1));
		else
			chessboard->bitBoard[rookType] ^= (1ull << move.src) | (1ull << (move.dst - 1));

		return;
	}

	if (move.isCapture) {
		uint64_t captureMask;
		if (!move.isEnPassant)
			captureMask = ~(1ull << move.dst);
		else
			captureMask = ~(1ull << chessboard->lastMove.dst);

		for (uint8_t i = 0; i < 12; ++i) chessboard->bitBoard[i] &= captureMask;
	}

	uint64_t moveMask = (1ull << move.src) | (1ull << move.dst);
	chessboard->bitBoard[move.srcPieceType] ^= moveMask;
	chessboard->lastMove = move;
}

bool Chessboard_squareAttacked(Chessboard *chessboard, uint8_t attacker, uint8_t squareLocation) {
	MovesArray *moves = Chessboard_computeAllMoves(chessboard, attacker, false, false);

	if (moves != NULL) {
		for (uint8_t i = 0; i < MovesArray_length(moves); ++i) {
			Move move = MovesArray_getMove(moves, i);

			if (move.dst == squareLocation) {
				MovesArray_destroy(moves);
				return true;
			}
		}

		MovesArray_destroy(moves);
	}

	return false;
}

bool Chessboard_kingInCheck(Chessboard *chessboard, uint8_t kingType) {
	uint64_t kingMask = chessboard->bitBoard[kingType];

	uint8_t attackedKingLocation = 0;
	while (kingMask) {
		if (kingMask & 1) break;
		attackedKingLocation++;
		kingMask >>= 1;
	}

	uint8_t attacker = kingType == 5 ? 1 : 0;

	MovesArray *moves = Chessboard_computeAllMoves(chessboard, attacker, false, false);

	if (moves != NULL) {
		for (uint8_t i = 0; i < MovesArray_length(moves); ++i) {
			Move move = MovesArray_getMove(moves, i);

			if (move.dst == attackedKingLocation) {
				MovesArray_destroy(moves);
				return true;
			}
		}
		MovesArray_destroy(moves);
	}

	return false;
}

void Chessboard_destroy(Chessboard *chessboard) {
	SDL_DestroyTexture(chessboard->spriteMap);

	free(chessboard);
}

bool Chessboard_isMoveLegal(Chessboard *chessboard, Move move) {
	uint8_t kingType = move.srcPieceType < 6 ? 5 : 11;

	Chessboard *newState = malloc(sizeof(Chessboard));
	memcpy(newState, chessboard, sizeof(Chessboard));
	newState->spriteMap = NULL;

	Chessboard_applyMove(newState, move);

	if (Chessboard_kingInCheck(newState, kingType)) {
		Chessboard_destroy(newState);
		return false;
	} else {
		Chessboard_destroy(newState);
		return true;
	}
}

bool Chessboard_isCheckmate(Chessboard *chessboard, uint8_t kingType) {
	uint8_t attacker = kingType == 5 ? 0 : 1;

	return Chessboard_kingInCheck(chessboard, kingType) && !Chessboard_hasLegalMoves(chessboard, attacker);
}

bool Chessboard_isStalemate(Chessboard *chessboard) {
	return (!Chessboard_kingInCheck(chessboard, 5) && !Chessboard_hasLegalMoves(chessboard, 0)) || (!Chessboard_kingInCheck(chessboard, 11) && !Chessboard_hasLegalMoves(chessboard, 1));
}

bool Chessboard_hasLegalMoves(Chessboard *chessboard, uint8_t attacker) {
	MovesArray *moves = Chessboard_computeAllMoves(chessboard, attacker, true, true);

	return moves != NULL && MovesArray_length(moves) > 0;
}

uint8_t Chessboard_countPieces(Chessboard *chessboard, uint8_t pieceType, uint8_t side) {
	uint64_t pieceMask = chessboard->bitBoard[side * 6 + pieceType];

	uint8_t count = 0;
	while (pieceMask) {
		if (pieceMask & 1) count++;
		pieceMask >>= 1;
	}
	return count;
}

uint64_t Chessboard_getPieceMask(Chessboard *chessboard, uint8_t pieceType, uint8_t side) {
	return chessboard->bitBoard[side * 6 + pieceType];
}
