#pragma once

#include <sdl3/SDL_audio.h>
#include <sdl3/SDL.h>
#include "../errors.h"

class audioEngine {
public:
	int initialize();

	SDL_AudioDeviceID device;
	SDL_AudioStream* stream;
	void play_default_sound();
};