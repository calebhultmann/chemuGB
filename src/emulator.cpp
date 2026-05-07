#pragma once

#include "emulator.h"
#include <SDL3/SDL_render.h>

chemuGB::chemuGB() {
	
}

chemuGB::~chemuGB() {

}

int chemuGB::initialize(std::filesystem::path romPath, uint8_t flags) {
	int status = engine.initialize(false);
	if (status != Error::None) {
		return status;
	}
	
	status = system.insertCartridge(romPath);
	if (status != Error::None) {
		return status;
	}

	last_input_poll = SDL_GetTicks();

	return 0;
}

void chemuGB::step() {
	uint64_t now = SDL_GetTicks();
	SDL_Event event;

	// TODO: This will continue to increase while the game is NOT stepping (debugging)
	//       Does this have negative effects (input every step)?
	if (now - last_input_poll >= INPUT_POLL_INTERVAL_MS) {
		last_input_poll = now;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				done = true;
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (event.key.scancode) {
					// P - Pause
				case SDL_SCANCODE_P:
					pause = !pause;
					break;
					// Q - Quit
				case SDL_SCANCODE_Q:
					done = true;
					break;
					// G - Green/Grayscale
				case SDL_SCANCODE_G:
					engine.palette = 1 - engine.palette;
					break;
					// W - D-pad Up
				case SDL_SCANCODE_W:
					system.joypad.press(KEY_UP);
					break;
					// A - D-pad Left
				case SDL_SCANCODE_A:
					system.joypad.press(KEY_LEFT);
					break;
					// S - D-pad Down
				case SDL_SCANCODE_S:
					system.joypad.press(KEY_DOWN);
					break;
					// D - D-pad Right
				case SDL_SCANCODE_D:
					system.joypad.press(KEY_RIGHT);
					break;
					// N - B Button
				case SDL_SCANCODE_N:
					system.joypad.press(KEY_B);
					break;
					// M - A button
				case SDL_SCANCODE_M:
					system.joypad.press(KEY_A);
					break;
					// V - Select
				case SDL_SCANCODE_V:
					system.joypad.press(KEY_SELECT);
					break;
					// B - Start
				case SDL_SCANCODE_B:
					system.joypad.press(KEY_START);
					break;
				}
				break;
			case SDL_EVENT_KEY_UP:
				switch (event.key.scancode) {
					// W - D-pad Up
				case SDL_SCANCODE_W:
					system.joypad.release(KEY_UP);
					break;
					// A - D-pad Left
				case SDL_SCANCODE_A:
					system.joypad.release(KEY_LEFT);
					break;
					// S - D-pad Down
				case SDL_SCANCODE_S:
					system.joypad.release(KEY_DOWN);
					break;
					// D - D-pad Right
				case SDL_SCANCODE_D:
					system.joypad.release(KEY_RIGHT);
					break;
					// N - B Button
				case SDL_SCANCODE_N:
					system.joypad.release(KEY_B);
					break;
					// M - A button
				case SDL_SCANCODE_M:
					system.joypad.release(KEY_A);
					break;
					// V - Select
				case SDL_SCANCODE_V:
					system.joypad.release(KEY_SELECT);
					break;
					// B - Start
				case SDL_SCANCODE_B:
					system.joypad.release(KEY_START);
					break;
				}
				break;
			}
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}
	}

	system.clock();
	// TODO: Should this go here or in console/renderer?
	/*if (system.ppu.is_frame_ready()) {
		engine.renderFrame(system.ppu.current_frame);
		drawDebug();
		SDL_RenderPresent(engine.dbg_renderer);
	}*/
}

void chemuGB::exit() {
	SDL_DestroyWindow(engine.window);
	SDL_Quit();
}