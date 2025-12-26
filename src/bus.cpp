#include "bus.h"

Bus::Bus() {
	cpu.connectBus(this);
	cart = std::make_shared<Cartridge>();
}

Bus::~Bus() {

}

int Bus::insertCartridge(const std::filesystem::path romPath) {
	return cart->loadCartridge(romPath);
}

uint8_t Bus::read(uint16_t addr) {
	if (boot && addr >= 0x0000 && addr <= 0x0100) {
		return dmg_boot[addr];
	}

	if (addr >= 0x0000 && addr <= 0x7FFF) {
		// rom  -  cartridge, mapper
		return cart->read(addr);
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		// vram  -  ppu? or bus
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		// external cartridge ram  -  cartridge, if available
		return cart->read(addr);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		// wram  -  cpu/bus
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		// mirror of C000 - DDFF (prohibited)
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		//OAM
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		// not usable
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		// I/O registers
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		//HRAM
			// FF80 - FFFE CPU WRAM or stack RAM
	}
	else if (addr == 0xFFFF) {
		//IE Flag
	}
	return 0;
}


void Bus::write(uint16_t addr, uint8_t data) {
	if (addr == 0xFF50) {
		boot = data;
	}
}