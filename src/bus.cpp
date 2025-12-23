#include "bus.h"

Bus::Bus() {
	cpu.connectBus(this);
}

Bus::~Bus() {

}

int Bus::insertCartridge(const std::filesystem::path romPath) {
	cart->loadCartridge(romPath);
}

uint8_t Bus::read(uint16_t addr) {
	if (addr >= 0x0000 && addr <= 0x7FFF) {
		// rom  -  cartridge, mapper
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		// vram  -  ppu? or bus
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		// external cartridge ram  -  cartridge, if available
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		// wram  -  cpu/bus
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		// mirror of C000 - DDFF (prohibited)
	}
	else if (addr >= 0xFE00 && addr <= 0xFFFF) {
		// FE00 - FFFF CPU internal RAM  -  cpu/bus
			// FE00 - FE9F OAM-RAM
			// FF00 - FF7F & FFFF - Specified regs and flags etc.
			// FF80 - FFFE CPU WRAM or stack RAM
	}
	return 0;
}


void Bus::write(uint16_t addr, uint8_t data) {

}