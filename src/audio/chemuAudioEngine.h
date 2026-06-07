#pragma once

#include <sdl3/SDL_audio.h>
#include <sdl3/SDL.h>
#include "../errors.h"
#include <chrono>
#include <array>

using Clock = std::chrono::steady_clock;

class APU;

class audioEngine {
public:
	const float dac_conversion = -2.0f / 15.0f;

	APU* apu = nullptr;

	float ch1_analog;
	float ch2_analog = 0;
	float ch3_analog = 0;
	float ch4_analog = 0;

	float left_analog;
	float right_analog;

	std::vector<float> samples;
	int initialize(APU*);

	void dacs();
	void mixer();
	void volume();

	void sample();
	void flush();
	void adjustRate();

	SDL_AudioDeviceID device;
	SDL_AudioStream* stream;
	void play_default_sound();
};