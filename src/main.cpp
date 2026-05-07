#include <iostream>
#include <filesystem>
#include "config.h"
#include "console.h"

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

	Config config;
	if (parse_args(config, argc, argv) != 0) {
		//return error
	}
	
	// handle IMMEDIATE errors here (like rom not existing)
	if (!std::filesystem::exists(config.filePath)) {
		std::cerr << "ROM not found in roms/\n";
		return Error::FileNotFound;
	}

	Console console(config);
	console.start();
	return 0;
}