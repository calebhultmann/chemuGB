#pragma once
#include <filesystem>

struct Config {
	bool debug = false;
	bool color = false;
	// breakpoint?
	// trace?
	std::filesystem::path filePath;
};

int parse_args(Config& config, int argc, char** argv);