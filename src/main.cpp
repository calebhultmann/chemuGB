#include <iostream>
#include "emulator.h"
#include <filesystem>
#include "config.h"

int main(int argc, char** argv) {
/*
	arg checks
	 - proper usage
	 - get flags (OR'd bits in a byte to stay efficient)
	 - get rom

	construct emulator
	 - use flags
	 - use rom
*/

	// TODO: LCD, Serial, Joypad interrupt requests are unimplemented

	if (argc < 2) {
		std::cout << "Usage: ./chemuGB.exe \"<ROM TITLE.gb>\"";
		return 1;
	}

	Config config = parse_args(argc, argv);
	
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "ROM not found in roms/\n";
		return Error::FileNotFound;
	}

	chemuGB gb;
	int init_status = gb.initialize(filePath, 0);
	if (init_status != Error::None) {
		return init_status;
	}





	gb.start();
	return 0;
}