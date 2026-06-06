#include "chemuAudioEngine.h"
#include "../core/apu.h"

int audioEngine::initialize(APU* a) {
	apu = a;

	// Initialize SDL3 Audio
	if (!SDL_Init(SDL_INIT_AUDIO)) {
		SDL_Log("SDL_VIDEO could not be initialized. Error: %s", SDL_GetError());
		SDL_Quit();
		return Error::NoSDLInit;
	}

	SDL_AudioSpec spec{};
	spec.freq = 44100;
	spec.format = SDL_AUDIO_F32LE;
	spec.channels = 2;

	device = SDL_OpenAudioDevice(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec
	);

	stream = SDL_CreateAudioStream(&spec, &spec);
	
	SDL_BindAudioStream(device, stream);


	time = Clock::now();
	debugtime = Clock::now();
	return 0;
}
#include <iostream>

void audioEngine::step() {
	using SampleDuration = std::chrono::duration<float>;
	constexpr SampleDuration sampleDuration{ 1.0f / 44100.0f };

	auto now = Clock::now();
	if (now - time < sampleDuration) {
		return;
	}

	dacs();
	mixer();
	volume();

	while (now - time >= sampleDuration) {
		time += std::chrono::duration_cast<std::chrono::nanoseconds>(sampleDuration);

		// Push Left Sample
		samples.push_back(left_analog);
		// Push Right Sample
		samples.push_back(right_analog);
	}

	SDL_PutAudioStreamData(
		stream,
		samples.data(),
		(int)samples.size() * sizeof(float)
	);

	samples.clear();
}

void audioEngine::dacs() {
	// Channel 1
	// Disable DAC "pulls" towards Analog 0, which is Digital 7.5
	float ch1_digital = (float)(apu->ch1.duty_value * apu->ch1.volume);
	float ch1_pre_analog = (apu->ch1.dac_enable ? ch1_digital : 7.5f);
	ch1_analog = ch1_pre_analog * dac_conversion + 1.0f;
}

void audioEngine::mixer() {
	left_analog = 0;
	right_analog = 0;

	// Left
	if (apu->nr51 & CH1_LEFT) {
		left_analog += ch1_analog;
	}
	if (apu->nr51 & CH2_LEFT) {
		left_analog += ch2_analog;
	}
	if (apu->nr51 & CH3_LEFT) {
		left_analog += ch3_analog;
	}
	if (apu->nr51 & CH4_LEFT) {
		left_analog += ch4_analog;
	}

	// Right
	if (apu->nr51 & CH1_RIGHT) {
		right_analog += ch1_analog;
	}
	if (apu->nr51 & CH2_RIGHT) {
		right_analog += ch2_analog;
	}
	if (apu->nr51 & CH3_RIGHT) {
		right_analog += ch3_analog;
	}
	if (apu->nr51 & CH4_RIGHT) {
		right_analog += ch4_analog;
	}
}

void audioEngine::volume() {
	uint8_t left = (apu->nr50 & VOLUME_LEFT) >> 4;
	uint8_t right = apu->nr50 & VOLUME_RIGHT;
	left_analog *= (float)((left + 1) / 8);
	right_analog *= (float)((right + 1) / 8);
}