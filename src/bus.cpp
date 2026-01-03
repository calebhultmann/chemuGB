#include "bus.h"

Bus::Bus() {
	cpu.connectBus(this);
	ppu.connectBus(this);
	ppu.fifo.connectBus(this);
	cart = std::make_shared<Cartridge>();
}

Bus::~Bus() {

}

int Bus::insertCartridge(const std::filesystem::path romPath) {
	return cart->loadCartridge(romPath);
}

void Bus::clock() {
	div++;

	if (tac & 0b00000100) {
		bool inc = false;
		switch (tac & 0b00000011) {
		case 0b00:
			inc = ((div & 0x3FF) == 0);
			break;
		case 0b01:
			inc = ((div & 0xF) == 0);
			break;
		case 0b10:
			inc = ((div & 0x3F) == 0);
			break;
		case 0b11:
			inc = ((div & 0xFF) == 0);
			break;
		}
		if (inc) {
			if (tima == 0xFF) {
				tima = tma;
				interrupts |= 0b00000100;
			}
			else {
				tima++;
			}
		}
	}
	cpu.clock();
	if (lcdc & 0b10000000) {
		ppu.clock();
	}
}

// TODO: Ensure correct R/W permissions for EVERY register
uint8_t Bus::readIOregs(uint16_t addr) {
	switch (addr) {
	case 0xFF00: return joyp;
	case 0xFF01: return sb;
	case 0xFF02: return sc;
	case 0xFF04: return div;
	case 0xFF05: return tima;
	case 0xFF06: return tma;
	case 0xFF07: return tac;
	case 0xFF0F: return interrupts;
	case 0xFF10: return audio_regs.nr10;
	case 0xFF11: return audio_regs.nr11;
	case 0xFF12: return audio_regs.nr12;
	case 0xFF13: return audio_regs.nr13;
	case 0xFF14: return audio_regs.nr14;
	case 0xFF16: return audio_regs.nr21;
	case 0xFF17: return audio_regs.nr22;
	case 0xFF18: return audio_regs.nr23;
	case 0xFF19: return audio_regs.nr24;
	case 0xFF1A: return audio_regs.nr30;
	case 0xFF1B: return audio_regs.nr31;
	case 0xFF1C: return audio_regs.nr32;
	case 0xFF1D: return audio_regs.nr33;
	case 0xFF1E: return audio_regs.nr34;
	case 0xFF20: return audio_regs.nr41;
	case 0xFF21: return audio_regs.nr42;
	case 0xFF22: return audio_regs.nr43;
	case 0xFF23: return audio_regs.nr44;
	case 0xFF24: return audio_regs.nr50;
	case 0xFF25: return audio_regs.nr51;
	case 0xFF26: return audio_regs.nr52;
	case 0xFF40: return lcdc;
	case 0xFF41: return stat;
	case 0xFF42: return scy;
	case 0xFF43: return scx;
	case 0xFF44: return ly;
	case 0xFF45: return lyc;
	case 0xFF46: return dma;
	case 0xFF47: return bgp;
	case 0xFF48: return obp0;
	case 0xFF49: return obp1;
	case 0xFF4A: return wy;
	case 0xFF4B: return wx;
	case 0xFF50: return bank;

	default:
		if (addr >= 0xFF30 && addr <= 0xFF3F) {
			return wave_ram[addr - 0xFF30];
		}
	}
	return 0;
}

void Bus::writeIOregs(uint16_t addr, uint8_t data) {
	switch (addr) {
	case 0xFF00: joyp = data; break;
	case 0xFF01: sb = data; break;
	case 0xFF02: sc = data; break;
	case 0xFF04: div = data; break;
	case 0xFF05: tima = data; break;
	case 0xFF06: tma = data; break;
	case 0xFF07: tac = data; break;
	case 0xFF0F: interrupts = data; break;
	case 0xFF10: audio_regs.nr10 = data; break;
	case 0xFF11: audio_regs.nr11 = data; break;
	case 0xFF12: audio_regs.nr12 = data; break;
	case 0xFF13: audio_regs.nr13 = data; break;
	case 0xFF14: audio_regs.nr14 = data; break;
	case 0xFF16: audio_regs.nr21 = data; break;
	case 0xFF17: audio_regs.nr22 = data; break;
	case 0xFF18: audio_regs.nr23 = data; break;
	case 0xFF19: audio_regs.nr24 = data; break;
	case 0xFF1A: audio_regs.nr30 = data; break;
	case 0xFF1B: audio_regs.nr31 = data; break;
	case 0xFF1C: audio_regs.nr32 = data; break;
	case 0xFF1D: audio_regs.nr33 = data; break;
	case 0xFF1E: audio_regs.nr34 = data; break;
	case 0xFF20: audio_regs.nr41 = data; break;
	case 0xFF21: audio_regs.nr42 = data; break;
	case 0xFF22: audio_regs.nr43 = data; break;
	case 0xFF23: audio_regs.nr44 = data; break;
	case 0xFF24: audio_regs.nr50 = data; break;
	case 0xFF25: audio_regs.nr51 = data; break;
	case 0xFF26: audio_regs.nr52 = data; break;
	case 0xFF40: lcdc = data; break;
	case 0xFF41: stat = data; break;
	case 0xFF42: scy = data; break;
	case 0xFF43: scx = data; break;
	case 0xFF44: ly = data; break;
	case 0xFF45: lyc = data; break;
	case 0xFF46: dma = data; break;
	case 0xFF47: bgp = data; break;
	case 0xFF48: obp0 = data; break;
	case 0xFF49: obp1 = data; break;
	case 0xFF4A: wy = data; break;
	case 0xFF4B: wx = data; break;
	case 0xFF50: bank = data; break;

	default:
		if (addr >= 0xFF30 && addr <= 0xFF3F) {
			wave_ram[addr - 0xFF30] = data;
		}
	}
}

uint8_t Bus::read(uint16_t addr) {
	if (bank == 0 && addr >= 0x0000 && addr <= 0x0100) {
		return dmg_boot[addr];
	}

	if (addr >= 0x0000 && addr <= 0x7FFF) {
		return cart->read(addr);
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		return ppu.read(addr);
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return cart->read(addr);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		return WRAM[addr - 0xC000];
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		// mirror of C000 - DDFF (prohibited)
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		return ppu.read(addr);
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		// not usable
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		return readIOregs(addr);
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		return HRAM[addr - 0xFF80];
	}
	else if (addr == 0xFFFF) {
		return ie;
	}
	return 0;
}


void Bus::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x0000 && addr <= 0x7FFF) {
		cart->write(addr, data);
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		ppu.write(addr, data);
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		cart->write(addr, data);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		WRAM[addr - 0xC000] = data;
	}
	else if (addr >= 0xE000 && addr <= 0xFDFF) {
		// mirror of C000 - DDFF (prohibited)
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		ppu.write(addr, data);
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		 // not usable
	}
	else if (addr >= 0xFF00 && addr <= 0xFF7F) {
		writeIOregs(addr, data);
	}
	else if (addr >= 0xFF80 && addr <= 0xFFFE) {
		HRAM[addr - 0xFF80] = data;
	}
	else if (addr == 0xFFFF) {
		ie = data;
	}
}