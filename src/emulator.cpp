#pragma once

#include "emulator.h"
#include <iostream>

chemuGB::chemuGB() {

}

chemuGB::~chemuGB() {

}

bool chemuGB::initialize() {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		return false;
	}
	window = SDL_CreateWindow("Test SDL Window", 1280, 1152, 0);
	if (window == NULL) {
		SDL_Log("Window could not be created! %s", SDL_GetError());
		SDL_Quit();
		return false;
	}


	renderer = SDL_CreateRenderer(window, NULL);

	for (int i = 0; i < 160; i++) {
		for (int j = 0; j < 144; j++) {
			SDL_SetRenderDrawColor(renderer, 100, i, j, 255);
			SDL_FRect rect = { float(i * 8), float(j * 8), float(8), float(8) };
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	SDL_RenderPresent(renderer);

	return true;
}

void chemuGB::start() { 
	for (int y = 0; y < 48; y++) {
		if (y % 8 == 0) {
			std::cout << "\n";
		}
		for (int x = 0; x < 16; x++) {
			cpe::Pixel current = font.getPixel(x, y);

			if (current.b == 255) {
				std::cout << ".";
			}
			else {
				std::cout << "0";
			}
		}
		std::cout << "\n";
	}
	
	bool done = false;
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