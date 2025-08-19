#pragma once
#include <string>
#include <vector>

constexpr size_t ROM_BANK_SIZE = 0x4000; // 16 KiB
constexpr size_t RAM_BANK_SIZE = 0x2000; // 8 KiB

class Cartridge
{
public:
	Cartridge(const std::string& filename);
	~Cartridge();

	std::vector<std::vector<uint8_t>> romBanks; // 0x4000 per bank
	std::vector<std::vector<uint8_t>> ramBanks; // 0x2000 per bank


	//rom, external ram, mbc

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