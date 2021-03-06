#include "AI.h"

static int32_t pieceWeights[] = { 100, 350, 350, 525, 1000, 10000 };
static int32_t pieceTables[6][64] = {
	{
		 0,  0,  0,  0,  0,  0,  0,  0,
		 5, 10, 10,-20,-20, 10, 10,  5,
		 5, -5,-10,  0,  0,-10, -5,  5,
		 0,  0,  0, 20, 20,  0,  0,  0,
		 5,  5, 10, 25, 25, 10,  5,  5,
		10, 10, 20, 30, 30, 20, 10, 10,
		50, 50, 50, 50, 50, 50, 50, 50,
		 0,  0,  0,  0,  0,  0,  0,  0
	},
	{
		0,  0,  0,  5,  5,  0,  0,  0,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		-5,  0,  0,  0,  0,  0,  0, -5,
		5, 10, 10, 10, 10, 10, 10,  5,
		0,  0,  0,  0,  0,  0,  0,  0
	},
	{
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	},
	{
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  5,  0,  0,  0,  0,  5,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	},
	{
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  5,  5,  5,  5,  5,  0,-10,
		  0,  0,  5,  5,  5,  5,  0, -5,
		 -5,  0,  5,  5,  5,  5,  0, -5,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},
	{
		 20, 30, 10,  0,  0, 10, 30, 20,
		 20, 20,  0,  0,  0,  0, 20, 20,
		-10,-20,-20,-20,-20,-20,-20,-10,
		-20,-30,-30,-40,-40,-30,-30,-20,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30,
		-30,-40,-40,-50,-50,-40,-40,-30
	}
};

static int32_t ChessAI_evaluate(Chessboard *chessboard) {
	int32_t score = 0;

	for (uint8_t pieceType = 0; pieceType < 6; ++pieceType) {
		int32_t deltaMaterial = Chessboard_countPieces(chessboard, pieceType, 0) - Chessboard_countPieces(chessboard, pieceType, 1);
		score += pieceWeights[pieceType] * deltaMaterial;

		uint64_t whiteMask = Chessboard_getPieceMask(chessboard, pieceType, 0);
		uint64_t blackMask = Chessboard_getPieceMask(chessboard, pieceType, 1);

		for (uint8_t i = 0; i < 64; ++i) {
			if (whiteMask & (1ull << i))
				score += pieceTables[pieceType][i];

			if (blackMask & (1ull << i))
				score -= pieceTables[pieceType][VMIRROR(i)];
		}
	}

	return score;
}

static int32_t ChessAI_MiniMax(Chessboard *chessboard, uint8_t depth, int32_t alpha, int32_t beta, uint8_t movingSide) {
	if (depth == 0 || Chessboard_isCheckmate(chessboard, 5) || Chessboard_isCheckmate(chessboard, 11) || Chessboard_isStalemate(chessboard)) return ChessAI_evaluate(chessboard);

	if (movingSide == 0) { // White
		int32_t bestScore = INT32_MIN;

		MovesArray *allMoves = Chessboard_computeAllMoves(chessboard, movingSide, true, true);
		for (uint8_t i = 0; i < MovesArray_length(allMoves); ++i) {
			Chessboard *newChessboard = malloc(sizeof(Chessboard));
			memcpy(newChessboard, chessboard, sizeof(Chessboard));
			newChessboard->spriteMap = NULL;

			Move move = MovesArray_getMove(allMoves, i);

			Chessboard_applyMove(newChessboard, move);

			int32_t moveScore = ChessAI_MiniMax(newChessboard, depth - 1, alpha, beta, 1);

			Chessboard_destroy(newChessboard);

			if (moveScore > bestScore) bestScore = moveScore;
			if (moveScore > alpha) alpha = moveScore;
			if (beta <= alpha) break;
		}
		MovesArray_destroy(allMoves);

		return bestScore;
	} else {
		int32_t bestScore = INT32_MAX;

		MovesArray *allMoves = Chessboard_computeAllMoves(chessboard, movingSide, true, true);
		for (uint8_t i = 0; i < MovesArray_length(allMoves); ++i) {
			Chessboard *newChessboard = malloc(sizeof(Chessboard));
			memcpy(newChessboard, chessboard, sizeof(Chessboard));
			newChessboard->spriteMap = NULL;

			Move move = MovesArray_getMove(allMoves, i);

			Chessboard_applyMove(newChessboard, move);

			int32_t moveScore = ChessAI_MiniMax(newChessboard, depth - 1, alpha, beta, 0);

			Chessboard_destroy(newChessboard);

			if (moveScore < bestScore) bestScore = moveScore;
			if (moveScore < beta) beta = moveScore;
			if (beta <= alpha) break;
		}
		MovesArray_destroy(allMoves);

		return bestScore;
	}
}

extern void ChessAI_nextMove(Chessboard *chessboard) {
	Move bestMove;
	int32_t bestScore = INT32_MAX;

	int32_t alpha = INT32_MIN;
	int32_t beta = INT32_MAX;

	MovesArray *allMoves = Chessboard_computeAllMoves(chessboard, 1, true, true);
	for (uint8_t i = 0; i < MovesArray_length(allMoves); ++i) {
		Chessboard *newChessboard = malloc(sizeof(Chessboard));
		memcpy(newChessboard, chessboard, sizeof(Chessboard));
		newChessboard->spriteMap = NULL;

		Move move = MovesArray_getMove(allMoves, i);

		Chessboard_applyMove(newChessboard, move);

		int32_t moveScore = ChessAI_MiniMax(newChessboard, MOVE_GENERATION_DEPTH - 1, alpha, beta, 0);

		Chessboard_destroy(newChessboard);

		if (moveScore < bestScore) {
			bestScore = moveScore;
			bestMove = move;
		}
		if (moveScore < beta) beta = moveScore;
		if (beta <= alpha) break;
	}
	MovesArray_destroy(allMoves);

	Chessboard_applyMove(chessboard, bestMove);
}
