#include <iostream>
#include "emulator.h"
#include <filesystem>

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

	// TODO: Implement all instructions (ensure 90+% correctness, blargg will test rest)
	// Blarg 2 (EI delay) incorrect, 3, 7, 8 all fault
	// TODO: Ensure correctness with registers

	if (argc < 2) {
		std::cout << "Usage: ./chemuGB.exe \"<ROM TITLE.gb>\"";
		return 1;
	}

	char* rom = argv[argc - 1];
	std::cout << rom << "\n";


	std::filesystem::path filePath = std::filesystem::path(__FILE__).parent_path();
	filePath /= "..\\roms";
	filePath /= rom;
	
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