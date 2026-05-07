#include "config.h"
#include <iostream>

int parse_args(Config& config, int argc, char** argv) {
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

	return 0;
}