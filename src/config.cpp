#include "config.h"
#include <iostream>

Config parse_args(int argc, char** argv) {
	Config config;
	for (int i = 1; i < argc - 1; i++) {
		if (argv[i] == "--debug") {
			config.debug = true;
		}
	}

	char* rom = argv[argc - 1];
	std::cout << rom << "\n";

	config.filePath = std::filesystem::path(__FILE__).parent_path();
	config.filePath /= "..\\roms";
	config.filePath /= rom;

	return config
}