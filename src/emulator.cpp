#pragma once

#include "emulator.h"
#include <SDL3/SDL_render.h>

chemuGB::chemuGB() {
	
}

chemuGB::~chemuGB() {

}

int chemuGB::initialize(std::filesystem::path romPath, uint8_t flags) {
	int status = system.insertCartridge(romPath);
	if (status != Error::None) {
		return status;
	}

	return 0;
}

void chemuGB::step() {
	system.clock();
}

bool chemuGB::handle_event(SDL_Event& event) {
	switch (event.type) {
		//TODO: Make controls changeable, or at least abstracted elsewhere
	case SDL_EVENT_KEY_DOWN:
		switch (event.key.scancode) {
			// W - D-pad Up
		case SDL_SCANCODE_W:
			system.joypad.press(KEY_UP);
			return true;
			// A - D-pad Left
		case SDL_SCANCODE_A:
			system.joypad.press(KEY_LEFT);
			return true;
			// S - D-pad Down
		case SDL_SCANCODE_S:
			system.joypad.press(KEY_DOWN);
			return true;
			// D - D-pad Right
		case SDL_SCANCODE_D:
			system.joypad.press(KEY_RIGHT);
			return true;
			// N - B Button
		case SDL_SCANCODE_N:
			system.joypad.press(KEY_B);
			return true;
			// M - A button
		case SDL_SCANCODE_M:
			system.joypad.press(KEY_A);
			return true;
			// V - Select
		case SDL_SCANCODE_V:
			system.joypad.press(KEY_SELECT);
			return true;
			// B - Start
		case SDL_SCANCODE_B:
			system.joypad.press(KEY_START);
			return true;
		}
		break;
	case SDL_EVENT_KEY_UP:
		switch (event.key.scancode) {
			// W - D-pad Up
		case SDL_SCANCODE_W:
			system.joypad.release(KEY_UP);
			return true;
			// A - D-pad Left
		case SDL_SCANCODE_A:
			system.joypad.release(KEY_LEFT);
			return true;
			// S - D-pad Down
		case SDL_SCANCODE_S:
			system.joypad.release(KEY_DOWN);
			return true;
			// D - D-pad Right
		case SDL_SCANCODE_D:
			system.joypad.release(KEY_RIGHT);
			return true;
			// N - B Button
		case SDL_SCANCODE_N:
			system.joypad.release(KEY_B);
			return true;
			// M - A button
		case SDL_SCANCODE_M:
			system.joypad.release(KEY_A);
			return true;
			// V - Select
		case SDL_SCANCODE_V:
			system.joypad.release(KEY_SELECT);
			return true;
			// B - Start
		case SDL_SCANCODE_B:
			system.joypad.release(KEY_START);
			return true;
		}
		break;
	}
	return false;
}