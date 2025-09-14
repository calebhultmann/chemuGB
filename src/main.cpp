#include "cartridge.h"
#include <iostream>
#include "emulator.h"

int main(int argc, char** argv) {
	Cartridge cart("Pokemon Red JPN.gb");
	chemuGB gb;
	if (!gb.initialize()) {
		return 0;
	}
	gb.start();
	return 0;
}