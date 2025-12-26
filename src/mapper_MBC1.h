#pragma once
#include "mapper.h"

class Mapper_MBC1 : public Mapper {
public:
	Mapper_MBC1(int ROMbanks, int RAMbanks);
	~Mapper_MBC1();

	uint8_t RAM_Enable = 0;
	uint8_t ROM_Bank_Number = 0;
	uint8_t Banking_Mode = 0;
	uint8_t Bank_Number_Upper_Bits = 0;

public:
	int mapRead(uint16_t addr, uint16_t& mapped_addr) override;
	int mapWrite(uint16_t addr, uint16_t& mapped_addr, uint8_t data) override;
};