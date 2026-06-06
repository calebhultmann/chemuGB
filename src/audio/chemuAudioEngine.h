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
	Clock::time_point debugtime;
	int hz = 0;
	const float dac_conversion = -2.0f / 15.0f;



	Clock::time_point time;
	const float sampleTime = 1 / 44100;
	APU* apu = nullptr;
	double audio_accumulator = 0;

	float sample;
	float ch1_analog;
	float ch2_analog = 0;
	float ch3_analog = 0;
	float ch4_analog = 0;

	float left_analog;
	float right_analog;

	std::vector<float> samples;
	int initialize(APU*);
	void step();

	void dacs();
	void mixer();
	// volume and possible HPF vvv
	void volume();
	void output();

	SDL_AudioDeviceID device;
	SDL_AudioStream* stream;
	void play_default_sound();
};