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

	sample_channels();

	while (now - time >= sampleDuration) {
		time += std::chrono::duration_cast<std::chrono::nanoseconds>(sampleDuration);
		
		//if (++hz == 44100) {
		//	hz = 0;
		//	double elapsed = std::chrono::duration<double>(now - debugtime).count();
		//	debugtime = now;
		//	std::cout << "elapsed: " << elapsed << '\n';
		//}

		// Push Left Sample
		samples.push_back(left_sample);
		// Push Right Sample
		samples.push_back(right_sample);
	}

	SDL_PutAudioStreamData(
		stream,
		samples.data(),
		(int)samples.size() * sizeof(float)
	);

	samples.clear();
}

void audioEngine::sample_channels() {
	// Channel 1
	if (apu->nr52 & CH1_ON) {
		int value = apu->ch1.duty_value;
		int volume = apu->ch1.volume;
		float real_volume = volume * (-2.0f / 15.0f);
		real_volume += 1.0f;
		
		ch1_sample =
			value ? real_volume : -1.0f;
		
		left_sample = ch1_sample;
		right_sample = ch1_sample;
	}
	else {
		left_sample = 0.0f;
		right_sample = 0.0f;
	}
}





#include <vector>
#include <numbers>
#include <cmath>
void audioEngine::play_default_sound() {
	constexpr int sample_rate = 44000;
	constexpr float frequency = 880.0f;
	std::vector<float> samples;

	for (int i = 0; i < sample_rate; i++) {
		float t = static_cast<float>(i) / sample_rate;

		float sample =
			std::sin(2.0f * std::numbers::pi_v<float>
				*frequency * t);

		samples.push_back(sample);
		samples.push_back(sample);
	}

	SDL_PutAudioStreamData(
		stream,
		samples.data(),
		(int)(samples.size() * sizeof(float)));
}