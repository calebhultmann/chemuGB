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

	return 0;
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
	left_analog *= (float)((float)(left + 1) / 8.0f);
	right_analog *= (float)((float)(right + 1) / 8.0f);
}


void audioEngine::sample() {
	dacs();
	mixer();
	volume();

	samples.push_back(left_analog);
	samples.push_back(right_analog);
}

void audioEngine::flush() {
	while (SDL_GetAudioStreamQueued(stream) > (int)(sizeof(float) * 2 * 44100 * 0.15f)) {
		SDL_Delay(1);
	}

	SDL_PutAudioStreamData(
		stream,
		samples.data(),
		(int)samples.size() * sizeof(float)
	);

	samples.clear();
}

void audioEngine::adjustRate() {
	int queued = SDL_GetAudioStreamQueued(stream);
	float bufferedSeconds = queued / (sizeof(float) * 2 * 44100.0f);

	// Target ~50ms of buffer — enough headroom without noticeable lag
	constexpr float targetBuffer = 0.015f;
	float error = bufferedSeconds - targetBuffer;

	// Nudge cyclesPerSample — positive error = running fast = slow down intake
	apu->cyclesPerSample = (4194304.0 / 44100.0) * (1.0 + error * 0.02);
}