#pragma once

#include "chemuPixelEngine.h"


class chemuGB : public cpe::pixelEngine {
public:
	chemuGB();
	~chemuGB();

public:
	bool initialize();
	void start();
};