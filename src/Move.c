#include "Move.h"
#include <stdio.h>

MovesArray *MovesArray_create() {
	MovesArray *mArr = malloc(sizeof(MovesArray));

	mArr->length = 0;
	mArr->capacity = 5;
	mArr->moves = malloc(mArr->capacity * sizeof(Move));

	return mArr;
}

void MovesArray_pushMove(MovesArray *mArr, Move move) {
	if (mArr->length == mArr->capacity) {
		mArr->capacity += 5;
		mArr->moves = realloc(mArr->moves, mArr->capacity * sizeof(Move));
	}

	mArr->moves[mArr->length++] = move;
}

Move MovesArray_getMove(MovesArray *mArr, uint32_t index) {
	assert(index < mArr->length);

	return mArr->moves[index];
}

uint32_t MovesArray_length(MovesArray *mArr) {
	return mArr->length;
}

void MovesArray_destroy(MovesArray *mArr) {
	free(mArr->moves);
	mArr->moves = NULL;

	free(mArr);
}
