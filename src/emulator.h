#pragma once

#include "chemuPixelEngine.h"
#include "bus.h"
#include <filesystem>

class chemuGB {

public:
	cpe::pixelEngine engine;
	Bus system;

	chemuGB();
	~chemuGB();

// Flags and attributes
private:
	int DEBUG = 0;
	int COLOR = 0;
	int SCALE = 8;

public:
	int initialize(std::filesystem::path romPath, uint8_t flags);
	void start();

public:	

	void drawDebugRegs();
	void drawDebug();

};