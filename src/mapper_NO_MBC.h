#pragma once
#include "mapper.h"

class Mapper_NO_MBC : public Mapper {
public:
	Mapper_NO_MBC(int ROMbanks, int RAMbanks);
	~Mapper_NO_MBC();

public:
	int mapRead(uint16_t addr, uint16_t& mapped_addr) override;
	int mapWrite(uint16_t addr, uint16_t& mapped_addr, uint8_t data) override;
};