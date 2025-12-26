#include "cartridge.h"
#include <fstream>
#include <iostream>

#include "mapper_MBC1.h"

Cartridge::Cartridge() {

}

Cartridge::~Cartridge() {

}

int Cartridge::loadCartridge(const std::filesystem::path romPath) {
	std::ifstream ifs;
	ifs.open(romPath, std::ifstream::binary);
	if (!ifs.is_open()) {
		return Error::BadFile;
	}

	ifs.seekg(0x100);
	ifs.read((char*)&header, sizeof(header));

	// Set up ROM banks
	if (header.rom_size > 8) {
		std::cerr << "Invalid ROM size detected\n";
		return Error::InvalidROMSize;
	}
	nROMbanks = (1 << header.rom_size) * 2;
	romBanks.resize(nROMbanks, std::vector<uint8_t>(ROM_BANK_SIZE));

	// Set up RAM banks
	if (header.ram_size == 1 || header.ram_size > 5) {
		std::cerr << "Invalid RAM size detected\n";
		return Error::InvalidRAMSize;
	}

	nRAMbanks = 0;
	switch (header.ram_size) {
	case 0: nRAMbanks = 0; break;
	case 2: nRAMbanks = 1; break;
	case 3: nRAMbanks = 4; break;
	case 4: nRAMbanks = 16; break;
	case 5: nRAMbanks = 8; break;
	}
	ramBanks.resize(nRAMbanks, std::vector<uint8_t>(RAM_BANK_SIZE));

	// Set up Mapper
	switch (header.cartridge_type) {
	case 0x00: break;
	case 0x01:
	case 0x02:
	case 0x03: mapper = std::make_shared<Mapper_MBC1>(nROMbanks, nRAMbanks); break;
	case 0x05:
	case 0x06:
	case 0x0B:
	case 0x0C:
	case 0x0D:
	case 0x0F:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
	case 0x20:
	case 0x22: std::cerr << "Unsupported mapper detected\n"; return Error::UnsupportedMapper;
	default: std::cerr << "Invalid mapper detected\n"; return Error::InvalidMapper;
	}

	// Read ROM data
	ifs.seekg(0);
	for (int i = 0; i < nROMbanks; i++) {
		ifs.read((char*)romBanks[i].data(), ROM_BANK_SIZE);
	}


	// Debugging info:
	ifs.seekg(0, ifs.end);
	int len = int(ifs.tellg());
	std::cout << "Length of rom: 0x" << std::hex << len << "\n";
	std::cout << std::dec << "ROM banks: " << nROMbanks << ", RAM banks: " << nRAMbanks << "\n\n";

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

	return 0;
}

uint8_t Cartridge::read(uint16_t addr) {
	uint16_t mapped_addr;
	uint8_t bank = mapper->mapRead(addr, mapped_addr);
	
	if (addr >= 0x0000 && addr <= 0x7FFF) {
		bank = bank & (nROMbanks - 1);
		return romBanks[bank][mapped_addr];
	}

	bank = bank & (nRAMbanks - 1);
	return ramBanks[bank][mapped_addr];
}