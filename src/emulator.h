#pragma once

#include "renderer/chemuPixelEngine.h"
#include "bus.h"
#include <filesystem>

class chemuGB {

public:
	cpe::pixelEngine engine;
	Bus system;

	chemuGB();
	~chemuGB();

	const uint64_t INPUT_POLL_INTERVAL_MS = 2;
	uint64_t last_input_poll;

	// how to get rid of these
	bool pause = false;
	bool done = false;

public:
	int initialize(std::filesystem::path romPath, uint8_t flags);
	void step();
	void exit();

};