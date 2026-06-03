#pragma once

#include <sdl3/SDL_audio.h>
#include <sdl3/SDL.h>
#include "../errors.h"
#include <chrono>
#include <array>

using Clock = std::chrono::steady_clock;

class audioEngine {
public:
	Clock::time_point time;
	const float sampleTime = 1 / 44100;
	float sample;

	float ch1_sample;
	float ch2_sample;
	float ch3_sample;
	float ch4_sample;

	float left;
	float right;

	std::array<float, 2> samples;

	int initialize();

	void step();

	void sample_channels(); // might want 4 functions
	void mix();
	// volume and possible HPF vvv
	void left();
	void right();
	void stream();


	SDL_AudioDeviceID device;
	SDL_AudioStream* stream;
	void play_default_sound();
};