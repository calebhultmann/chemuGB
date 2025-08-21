#pragma once
#include "mapper.h"

class Mapper_MBC1 : public Mapper {
public:
	Mapper_MBC1(int ROMbanks, int RAMbanks);
	~Mapper_MBC1();

public:
	bool read(uint16_t addr, uint16_t& mapped_addr) override;
	bool write(uint16_t addr, uint16_t& mapped_addr, uint8_t data) override;
};