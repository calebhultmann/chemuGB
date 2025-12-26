#pragma once
#include <cstdint>

class Mapper {
public:
	Mapper(int ROMbanks, int RAMbanks);
	~Mapper();

public:
	virtual int mapRead(uint16_t addr, uint16_t& mapped_addr) = 0;
	virtual int mapWrite(uint16_t addr, uint16_t& mapped_addr, uint8_t data) = 0;

	
protected:
	int nROMbanks = 0;
	int nRAMbanks = 0;
};