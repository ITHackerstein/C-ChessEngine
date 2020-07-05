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
	Chessboard *cb = createChessboard();

	while (1) {
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_QUIT) break;
		}

		drawChessboard(cb, screenSurf);
		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
