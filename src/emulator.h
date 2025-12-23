#pragma once

#include "chemuPixelEngine.h"
#include "bus.h"
#include <filesystem>

class chemuGB {
	// Add options for:
	//  - Optional debug mode
	//  - Gray/greenscale
public:
	cpe::pixelEngine engine;
	Bus system;

	chemuGB();
	~chemuGB();

	int initialize(std::filesystem::path romPath, uint8_t flags);
	void start();

	//void drawDebug();

};