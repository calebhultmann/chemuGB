#include "apu.h"
#include "bus.h"

void APU::clock() {
	if (bus->div & 0b00011111) {
		return;
	}

	div++;

	if (div % 2 == 0) {
		// sound length
	}
	
	if (div % 4 == 0) {
		// ch1 freq sweep
	}

	if (div % 8 == 0) {
		// envelope sweep
	}

}

/*
* Channel off IF:
* The channel’s length timer is enabled in NRx4 and expires
* For CH1 only: when the period sweep overflows
* The channel’s DAC is turned off.

* NOTE: The envelope reaching a volume of 0 does NOT turn the channel off!
*/

uint8_t APU::read(uint16_t addr) const {
	switch (addr) {
	case 0xFF10: return nr10;
	case 0xFF11: return nr11 & 0b11000000;
	case 0xFF12: return nr12;
	case 0xFF13: return 0xFF;
	case 0xFF14: return nr14 & 0b01000000;
	case 0xFF16: return nr21 & 0b11000000;
	case 0xFF17: return nr22;
	case 0xFF18: return 0xFF;
	case 0xFF19: return nr24 & 0b01000000;
	case 0xFF1A: return nr30;
	case 0xFF1B: return 0xFF;
	case 0xFF1C: return nr32;
	case 0xFF1D: return 0xFF;
	case 0xFF1E: return nr34 & 0b01000000;
	case 0xFF20: return 0xFF;
	case 0xFF21: return nr42;
	case 0xFF22: return nr43;
	case 0xFF23: return nr44 & 0b01000000;
	case 0xFF24: return nr50;
	case 0xFF25: return nr51;
	case 0xFF26: return nr52;
	}
}

void APU::write(uint16_t addr, uint8_t data) {
	// Exit if trying to write in read-only mode (APU is off)
	if (!(audio_regs.nr52 & 0b10000000) && (addr != 0xFF26)) {
		return;
	}

	switch (addr) {
	case 0xFF10: nr10 = data & 0b01111111; break;
	case 0xFF11: nr11 = data; break;
	case 0xFF12: nr12 = data; break;
	case 0xFF13: nr13 = data; break;
	case 0xFF14: nr14 = data & 0b11000111; break;
	case 0xFF16: nr21 = data; break;
	case 0xFF17: nr22 = data; break;
	case 0xFF18: nr23 = data; break;
	case 0xFF19: nr24 = data & 0b11000111; break;
	case 0xFF1A: nr30 = data & 0b10000000; break;
	case 0xFF1B: nr31 = data; break;
	case 0xFF1C: nr32 = data & 0b01100000; break;
	case 0xFF1D: nr33 = data; break;
	case 0xFF1E: nr34 = data & 0b11000111; break;
	case 0xFF20: nr41 = data & 0b00111111; break;
	case 0xFF21: nr42 = data; break;
	case 0xFF22: nr43 = data; break;
	case 0xFF23: nr44 = data & 0b11000000; break;
	case 0xFF24: nr50 = data; break;
	case 0xFF25: nr51 = data; break;
	case 0xFF26: nr52 = (data & 0b10000000) | (audio_regs.nr52 & 0b00001111); break;
	}
}