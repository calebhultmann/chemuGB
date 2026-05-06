#include <filesystem>

struct Config {
	bool debug = false;
	// breakpoint?
	// trace?
	std::filesystem::path filepath;
};

Config parse_args(int argc, char** argv);