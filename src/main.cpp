#include "cartridge.h"
#include <iostream>
#include "emulator.h"

int main(int argc, char** argv) {
	Cartridge cart("Pokemon Red JPN.gb");
	chemuGB gb;
	gb.start();
	return 0;
}