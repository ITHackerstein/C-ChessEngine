#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include "Chessboard.h"

const uint32_t WIDTH = 600;
const uint32_t HEIGHT = 600;

int main(int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Error during SDL2 initialization!: %s\n", SDL_GetError());
		return 1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	SDL_Window *window = SDL_CreateWindow("Chess Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "Erroring during creation of the window!: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface *screenSurf = SDL_GetWindowSurface(window);
	Chessboard *cb = Chessboard_create();

	uint8_t running = 1;
	while (running) {
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
				running = 0;
				break;

				case SDL_MOUSEBUTTONDOWN:
				if (evt.button.button == SDL_BUTTON_LEFT) {
					uint8_t col = evt.button.x * 8 / WIDTH;
					uint8_t row = 7 - evt.button.y * 8 / HEIGHT;

					MovesArray *moves = Chessboard_computePieceMoves(cb, row * 8 + col);
					for (uint32_t i = 0; i < MovesArray_length(moves); ++i) {
						Move move = MovesArray_getMove(moves, i);
						printf("%d %d\n", move.src, move.dst);
					}
				}
			}
		}

		Chessboard_draw(cb, screenSurf);
		SDL_UpdateWindowSurface(window);
	}


	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
