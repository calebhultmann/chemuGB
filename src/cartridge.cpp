#include "cartridge.h"
#include <filesystem>
#include <fstream>

Cartridge::Cartridge(const std::string& filename) {
	struct gbHeader {
		uint8_t	entry_point[4];
		uint8_t	logo[48];
		uint8_t	title[16]; // May also be title, manufacturer code, and cgb flag
		uint8_t	licensee_code[2];
		uint8_t	sgb_flag;
		uint8_t	cartridge_type;
		uint8_t	rom_size;
		uint8_t	ram_size;
		uint8_t	destination;
		uint8_t	old_licensee_code;
		uint8_t	version;
		uint8_t	header_checksum;
		uint8_t	global_checksum[2];
	} header;
	header = { 0 };
	std::ifstream ifs;
	std::filesystem::path filePath = std::filesystem::path(__FILE__).parent_path();

}