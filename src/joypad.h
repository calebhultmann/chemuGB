#pragma once
#include <cstdint>

enum key : uint8_t {
	KEY_RIGHT  = 0b00001,
	KEY_LEFT   = 0b00010,
	KEY_UP     = 0b00100,
	KEY_DOWN   = 0b01000,
	KEY_A      = 0b10001,
	KEY_B      = 0b10010,
	KEY_SELECT = 0b10100,
	KEY_START  = 0b11000,
	BUTTONS = 0b10000,
	KEY     = 0b1111,

	BUTTON_ENABLE = 0b00100000,
	DPAD_ENABLE   = 0b00010000
};

class Bus;

class Joypad {
public:
	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }

	uint8_t dpad = 0b1111; //0b0000DULR
	uint8_t buttons = 0b1111; // 0b0000SsBA

	void press(uint8_t button);
	void release(uint8_t button);
};