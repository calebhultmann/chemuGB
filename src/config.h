#include <filesystem>

struct Config {
	bool debug = false;
	// breakpoint?
	// trace?
	std::filesystem::path filePath;
};

int parse_args(Config& config, int argc, char** argv);