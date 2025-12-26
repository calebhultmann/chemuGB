#include "mapper_MBC1.h"

Mapper_MBC1::Mapper_MBC1(int ROMbanks, int RAMbanks) : Mapper(ROMbanks, RAMbanks) {

}

Mapper_MBC1::~Mapper_MBC1() {

}

int Mapper_MBC1::mapRead(uint16_t addr, uint16_t& mapped_addr) {
	if (addr >= 0x0000 && addr <= 0x3FFF) {
		mapped_addr = addr;
		if (Banking_Mode == 0) {
			return 0;
		}
		
		return Bank_Number_Upper_Bits << 5;
	}
	else if (addr >= 0x4000 && addr <= 0x7FFF) {
		mapped_addr = addr & 0x3FFF;
		return (Bank_Number_Upper_Bits << 5) | ROM_Bank_Number;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		mapped_addr = addr & 0x1FFF;
		if (Banking_Mode == 0) {
			return 0;
		}

		return Bank_Number_Upper_Bits;
	}

	return 0;
}

int Mapper_MBC1::mapWrite(uint16_t addr, uint16_t& mapped_addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x1FFF) {
		if ((data | 0xF) == 0xA) {
			RAM_Enable = 1;
		}
		else {
			RAM_Enable = 0;
		}
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF) {
		uint8_t bank = data & 0b00011111;
		if (bank == 0) {
			bank = 1;
		}
		ROM_Bank_Number = bank;
	}
	else if (addr >= 0x4000 && addr <= 0x5FFF) {
		uint8_t bank = data & 0b00000011;
		Bank_Number_Upper_Bits = bank;
	}
	else if (addr >= 0x6000 && addr <= 0x7FFF) {
		Banking_Mode = data & 0b00000001;
	}
	
	return 0;
}
