#include "mapper_MBC1.h"

Mapper_MBC1::Mapper_MBC1(int ROMbanks, int RAMbanks) : Mapper(ROMbanks, RAMbanks) {

}

Mapper_MBC1::~Mapper_MBC1() {

}

bool Mapper_MBC1::read(uint16_t addr, uint16_t& mapped_addr) {
	return false;
}

bool Mapper_MBC1::write(uint16_t addr, uint16_t& mapped_addr, uint8_t data) {
	return false;
}
