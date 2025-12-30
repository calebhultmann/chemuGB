#pragma once
#include <string>
#include <vector>
#include <memory>
#include "mapper.h"
#include "errors.h"
#include <filesystem>

constexpr size_t ROM_BANK_SIZE = 0x4000; // 16 KiB
constexpr size_t RAM_BANK_SIZE = 0x2000; // 8 KiB

class Cartridge
{
public:
	Cartridge();
	~Cartridge();

	int loadCartridge(const std::filesystem::path romPath);

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

	int nROMbanks;
	int nRAMbanks;

	std::vector<std::vector<uint8_t>> romBanks; // 0x4000 per bank
	std::vector<std::vector<uint8_t>> ramBanks; // 0x2000 per bank

	std::shared_ptr<Mapper> mapper;

	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	// Cartridge Types
	//$00	ROM ONLY
	//$01	MBC1
	//$02	MBC1 + RAM
	//$03	MBC1 + RAM + BATTERY
	//$05	MBC2
	//$06	MBC2 + BATTERY
	//$0B	MMM01
	//$0C	MMM01 + RAM
	//$0D	MMM01 + RAM + BATTERY
	//$0F	MBC3 + TIMER + BATTERY
	//$10	MBC3 + TIMER + RAM + BATTERY 10
	//$11	MBC3
	//$12	MBC3 + RAM 10
	//$13	MBC3 + RAM + BATTERY 10
	//$19	MBC5
	//$1A	MBC5 + RAM
	//$1B	MBC5 + RAM + BATTERY
	//$1C	MBC5 + RUMBLE
	//$1D	MBC5 + RUMBLE + RAM
	//$1E	MBC5 + RUMBLE + RAM + BATTERY
	//$20	MBC6
	//$22	MBC7 + SENSOR + RUMBLE + RAM + BATTERY
	//$FC	POCKET CAMERA
	//$FD	BANDAI TAMA5
	//$FE	HuC3
	//$FF	HuC1 + RAM + BATTERY
};