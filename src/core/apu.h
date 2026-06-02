#pragma once
#include <cstdint>
class Bus;

class APU
{
public:
	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }

	// Channel 1 - Pulse with period sweep
	uint8_t nr10; // Sweep
	uint8_t nr11; // Length Timer & Duty Cycle
	uint8_t nr12; // Volume & Envelope
	uint8_t nr13; // Period Low
	uint8_t nr14; // Period High & Control
	// Channel 2 - Pulse
	uint8_t nr21; // Length Timer & Duty Cycle
	uint8_t nr22; // Volume & Envelope
	uint8_t nr23; // Period Low
	uint8_t nr24; // Period High & Control
	// Channel 3 - Wave output
	uint8_t nr30; // DAC Enable
	uint8_t nr31; // Length Timer
	uint8_t nr32; // Output Level
	uint8_t nr33; // Period Low
	uint8_t nr34; // Period High & Control
	// Channel 4 - Noise
	uint8_t nr41; // Length Timer
	uint8_t nr42; // Volume & Envelope
	uint8_t nr43; // Frequency & Randomness
	uint8_t nr44; // Control

	uint8_t nr50; // Master Volume & VIN Panning
	uint8_t nr51; // Sound Panning
	uint8_t nr52; // Audio Master Control

	uint32_t div = 0;
	uint8_t read(uint16_t addr) const;
	void write(uint16_t addr, uint8_t data);

	void clock();
};