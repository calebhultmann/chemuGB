#pragma once
#include <cstdint>
#include <chrono>
#include "../audio/chemuAudioEngine.h"

using Clock = std::chrono::steady_clock;


// Master Macros
#define CH4_LEFT		0b10000000
#define CH3_LEFT		0b01000000
#define CH2_LEFT		0b00100000
#define CH1_LEFT		0b00010000
#define CH4_RIGHT		0b00001000
#define CH3_RIGHT		0b00000100
#define CH2_RIGHT		0b00000010
#define CH1_RIGHT		0b00000001
#define VOLUME_LEFT		0b01110000
#define VOLUME_RIGHT	0b00000111

// Channel 1 Macros
#define CH1_ON			0b00000001
#define CH1_OFF			0b11111110
#define CH1_TMR_ENA		0b01000000
#define CH1_TMR_DIS		0b01000000
#define CH1_TMR_INIT	0b00111111
#define CH1_PRD_HIGH	0b00000111
#define CH1_SWP_PACE	0b00000111
#define CH1_ENV_DIR		0b00001000

// Channel 2 Macros
#define CH2_ON			0b00000010
#define CH2_OFF			0b11111101
#define CH2_TMR_ENA		0b01000000
#define CH2_TMR_DIS		0b01000000
#define CH2_TMR_INIT	0b00111111
#define CH2_PRD_HIGH	0b00000111
#define CH2_SWP_PACE	0b00000111
#define CH2_ENV_DIR		0b00001000



// Channel 4 Macros
#define CH4_ON			0b00001000
#define CH4_OFF			0b11110111
#define CH4_TMR_ENA		0b01000000
#define CH4_TMR_DIS		0b01000000
#define CH4_TMR_INIT	0b00111111
#define CH4_SHORT_LSFR	0b00001000
#define CH4_SWP_PACE	0b00000111
#define CH4_ENV_DIR		0b00001000
#define CH4_LONG_BIT	0b1000000000000000
#define CH4_SHORT_BIT	0b10000000

static const uint8_t duty_table[4][8]{
	{0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,1,1,1},
	{0,1,1,1,1,1,1,0}
};

class Bus;

struct channel_1 {
	// Frequency Sweep
	bool freq_sweep_dir;

	// Length timer
	uint8_t len_timer = 0;

	// Wavy duty
	uint8_t duty_position = 0;
	bool duty_value = 0;

	// Volume & Envelope
	uint8_t volume = 0;
	bool env_dir;
	uint16_t env_sweep_timer = 0;

	// Period
	uint16_t period_value = 0;

	// DAC
	bool dac_enable = false;
};

struct channel_2 {
	// Length timer
	uint8_t len_timer = 0;

	// Wavy duty
	uint8_t duty_position = 0;
	bool duty_value = 0;

	// Volume & Envelope
	uint8_t volume = 0;
	bool env_dir;
	uint16_t env_sweep_timer = 0;

	// Period
	uint16_t period_value = 0;

	// DAC
	bool dac_enable = false;
};

struct channel_4 {
	// Length timer
	uint8_t len_timer = 0;

	// Volume & Envelope
	uint8_t volume = 0;
	bool env_dir;
	uint16_t env_sweep_timer = 0;

	// LSFR
	uint16_t lsfr = 0;
	bool lsfr_value = false;

	// Counter
	uint8_t divider = 0;
	uint8_t shift = 0;
	uint16_t shift_timer = 0;

	// DAC
	bool dac_enable = false;
};

class APU
{
public:
	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }
	
	audioEngine speaker;
	void initialize();
	void shutoff();

	double cyclesPerSample = 4194304.0 / 44100.0;
	double cycle_accumulator = 0;

	uint32_t div = 0;
	channel_1 ch1;
	channel_2 ch2;
	//channel_3 ch3;
	channel_4 ch4;

	
	uint8_t ch3_timer = 0;

	bool no_tick = false;

	// Channel 1 - Pulse with period sweep
	uint8_t nr10 = 0; // Sweep
	uint8_t nr11 = 0; // Length Timer & Duty Cycle
	uint8_t nr12 = 0; // Volume & Envelope
	uint8_t nr13 = 0; // Period Low
	uint8_t nr14 = 0; // Period High & Control
	// Channel 2 - Pulse
	uint8_t nr21 = 0; // Length Timer & Duty Cycle
	uint8_t nr22 = 0; // Volume & Envelope
	uint8_t nr23 = 0; // Period Low
	uint8_t nr24 = 0; // Period High & Control
	// Channel 3 - Wave output
	uint8_t nr30 = 0; // DAC Enable
	uint8_t nr31 = 0; // Length Timer
	uint8_t nr32 = 0; // Output Level
	uint8_t nr33 = 0; // Period Low
	uint8_t nr34 = 0; // Period High & Control
	// Channel 4 - Noise
	uint8_t nr41 = 0; // Length Timer
	uint8_t nr42 = 0; // Volume & Envelope
	uint8_t nr43 = 0; // Frequency & Randomness
	uint8_t nr44 = 0; // Control

	uint8_t nr50 = 0; // Master Volume & VIN Panning
	uint8_t nr51 = 0; // Sound Panning
	uint8_t nr52 = 0; // Audio Master Control

	uint8_t read(uint16_t addr) const;
	void write(uint16_t addr, uint8_t data);

	void clock();
};