#include "chemuAudioEngine.h"

int audioEngine::initialize() {
	// Initialize SDL3 Audio
	if (!SDL_Init(SDL_INIT_AUDIO)) {
		SDL_Log("SDL_VIDEO could not be initialized. Error: %s", SDL_GetError());
		SDL_Quit();
		return Error::NoSDLInit;
	}

	SDL_AudioSpec spec{};
	spec.freq = 44000;
	spec.format = SDL_AUDIO_F32LE;
	spec.channels = 2;

	device = SDL_OpenAudioDevice(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec
	);

	stream = SDL_CreateAudioStream(&spec, &spec);
	
	SDL_BindAudioStream(device, stream);

	return 0;
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