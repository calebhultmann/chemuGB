#pragma once

#include "emulator.h"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>

chemuGB::chemuGB() {

}

chemuGB::~chemuGB() {

}

void chemuGB::start() {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return;
	}
	bool done = false;
	SDL_Window* window = SDL_CreateWindow("Test SDL Window", 1280, 1152, 0);

	if (window == NULL) {
		SDL_Log("Window could not be created! %s", SDL_GetError());
		SDL_Quit();
		return;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	for (int i = 0; i < 160; i++) {
		for (int j = 0; j < 144; j++) {
			SDL_SetRenderDrawColor(renderer, 100, i, j, 255);
				SDL_FRect rect = { float(i * 8), float(j * 8), float(8), float(8) };
				SDL_RenderFillRect(renderer, &rect);

			if ((i + j) % 2 == 1) {
			}
		}
	}
	SDL_RenderPresent(renderer);

	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
}