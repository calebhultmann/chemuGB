#include "mapper_NO_MBC.h"

Mapper_NO_MBC::Mapper_NO_MBC(int ROMbanks, int RAMbanks) : Mapper(ROMbanks, RAMbanks) {

}

Mapper_NO_MBC::~Mapper_NO_MBC() {

}

int Mapper_NO_MBC::mapRead(uint16_t addr, uint16_t& mapped_addr) {
	if (addr >= 0x0000 && addr <= 0x3FFF) {
		mapped_addr = addr;
		return 0;
	}
	else {
		mapped_addr = addr & 0x3FFF;
		return 1;
	}
	return 0;
}

int Mapper_NO_MBC::mapWrite(uint16_t addr, uint16_t& mapped_addr, uint8_t data) {
	return 0;
}