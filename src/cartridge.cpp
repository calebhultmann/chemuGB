#include "cartridge.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

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

	std::ifstream ifs;
	std::filesystem::path filePath = std::filesystem::path(__FILE__).parent_path();
	filePath /= "..\\roms";
	filePath /= filename;
	ifs.open(filePath, std::ifstream::binary);
	if (!ifs.is_open()) {
		// file not found
		std::cout << "uh oh\n";
		return;
	}

	ifs.seekg(0, ifs.end);
	int len = int(ifs.tellg());
	std::cout << "Length of rom: " << std::hex << len << "\n\n";

	ifs.seekg(0x100);

	ifs.read((char*)&header, sizeof(header));

	std::cout << "Entry point: ";
	for (int i = 0; i < 4; i++) {
		std::cout << std::hex << int(header.entry_point[i]) << " ";
	} std::cout << '\n';
	std::cout << "Logo: ";
	for (int i = 0; i < 48; i++) {
		std::cout << std::hex << int(header.logo[i]) << " ";
	} std::cout << '\n';
	std::cout << "Title: ";
	for (int i = 0; i < 16; i++) {
		std::cout << std::hex << int(header.title[i]) << " ";
	} std::cout << '\n';
	std::cout << "Licensee code: ";
	for (int i = 0; i < 2; i++) {
		std::cout << std::hex << int(header.licensee_code[i]) << " ";
	} std::cout << '\n';

	std::cout << "SGB flag: " << std::hex << int(header.sgb_flag) << '\n';
	std::cout << "Cartridge type: " << std::hex << int(header.cartridge_type) << '\n';
	std::cout << "Rom size: " << std::hex << int(header.rom_size) << "\n";
	std::cout << "Ram size: " << std::hex << int(header.ram_size) << "\n";
	std::cout << "Destination: " << std::hex << int(header.destination) << "\n";
	std::cout << "Old licensee code: " << std::hex << int(header.old_licensee_code) << "\n";
	std::cout << "Version: " << std::hex << int(header.version) << "\n";
	std::cout << "Header checksum: " << std::hex << int(header.header_checksum) << "\n";
	std::cout << "Global checksum: ";
	for (int i = 0; i < 2; i++) {
		std::cout << std::hex << int(header.global_checksum[i]) << " ";
	} std::cout << '\n';
}

Cartridge::~Cartridge() {

}