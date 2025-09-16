#pragma once

#include "emulator.h"
#include <iostream>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL.h>

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
	font.toTexture(renderer, fontTexture);
	SDL_SetTextureScaleMode(fontTexture, SDL_SCALEMODE_NEAREST);

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
	DrawString(0, 0, "Hello, World!", cpe::Pixel{ 0,0,0 });

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