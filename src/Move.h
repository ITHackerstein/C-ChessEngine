#ifndef _MOVE_INCLUDED
#define _MOVE_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct {
	uint8_t src;
	uint8_t dst;
	bool isCapture;
} Move;

typedef struct {
	Move *moves;
	uint32_t capacity;
	uint32_t length;
} MovesArray;

extern MovesArray *MovesArray_create();
extern void MovesArray_pushMove(MovesArray *, uint8_t src, uint8_t dst, bool isCapture);
extern Move MovesArray_getMove(MovesArray *, uint32_t index);
extern uint32_t MovesArray_length(MovesArray *);
extern void *MovesArray_destroy(MovesArray *);

#endif
