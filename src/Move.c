#include "Move.h"

MovesArray *MovesArray_create() {
	MovesArray *movesArray = malloc(sizeof(MovesArray*));

	movesArray->length = 0;
	movesArray->capacity = 5;
	movesArray->moves = malloc(movesArray->capacity * sizeof(Move));

	return movesArray;
}

void MovesArray_pushMove(MovesArray *mArr, uint8_t src, uint8_t dst) {
	if (mArr->length == mArr->capacity) {
		mArr->capacity += 5;
		mArr->moves = realloc(mArr, mArr->capacity * sizeof(Move));
	}

	Move move = {.src = src, .dst = dst};
	mArr->moves[mArr->length++] = move;
}

Move MovesArray_getMove(MovesArray *mArr, uint32_t index) {
	assert(index < mArr->length);

	return mArr->moves[index];
}

uint32_t MovesArray_length(MovesArray *mArr) {
	return mArr->length;
}