#include "apu.h"
#include "bus.h"

#define FULL_TIMER	0b00111111
#define TRIGGER		0b10000000



void APU::clock() {
	if (bus->div & 0b00011111) {
		return;
	}

	div++;

	// Sound Length
	if (div % 2 == 0) {
		// If Channel 1 Length Enabled
		if ((nr14 & CH1_TMR_ENA) && (ch1.timer < CH1_TMR_DIS)) {
			if (++ch1.timer == CH1_TMR_DIS) {
				nr52 &= CH1_OFF;
			}
		}
	}
	
	// CH1 Freq Sweep
	if (div % 4 == 0) {

	}

	// Envelope Sweep
	if (div % 8 == 0) {
		// Channel 1 Sweep
		ch1.sweep_timer++;
		if (nr12 & CH1_SWP_PACE) {
			if (ch1.sweep_timer % (nr12 & CH1_SWP_PACE) == 0) {
				// NOTE: Logic will need to be added to control volume 0-15
				(nr12 & CH1_ENV_DIR) ? ch1.volume++ : ch1.volume--;
			}
		}
		// Channel 2 Sweep
		// Channel 3 Sweep
		// Channel 4 Sweep
	}
}

/*
* Channel off IF:
* The channel’s length timer is enabled in NRx4 and expires
* For CH1 only: when the period sweep overflows
* The channel’s DAC is turned off.

* NOTE: The envelope reaching a volume of 0 does NOT turn the channel off!

setting bit 7 of NRx4 turns channel x on


Channel x’s DAC is enabled if and only if [NRx2] & $F8 != 0;
the exception is CH3, whose DAC is directly controlled by bit 7 of NR30 instead.
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
	return 0xFF;
}

void APU::write(uint16_t addr, uint8_t data) {
	// Exit if trying to write in read-only mode (APU is off)
	if (!(nr52 & 0b10000000) && (addr != 0xFF26)) {
		return;
	}

	switch (addr) {
	// Channel 1
	case 0xFF10: // Sweep
		nr10 = data & 0b01111111; break;
	case 0xFF11: // Length Timer & Duty Cycle
		nr11 = data; break;
	case 0xFF12: // Volume & Envelope
		nr12 = data; break;
	case 0xFF13: // Period Low
		nr13 = data; break;
	case 0xFF14: // Period High & Control
		if (data & TRIGGER) {
			/*
			Envelope timer is reset. - ???
			Sweep does several things. - ???
			*/

			// Ch1 is enabled.
			nr52 |= CH1_ON;

			// If length timer expired it is reset.
			if (ch1.timer == CH1_TMR_DIS) {
				ch1.timer = (nr11 & CH1_TMR_INIT);
			}

			//The period divider is set to the contents of NR13 and NR14.
			ch1.period = nr14 & CH1_PRD_HIGH;
			ch1.period <<= 8;
			ch1.period &= nr13;

			// Volume is set to contents of NR12 initial volume.
			ch1.volume = (nr12 >> 4);
		}
		
		nr14 = data & 0b11000111; break;

	// Channel 2
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
	case 0xFF26: nr52 = (data & 0b10000000) | (nr52 & 0b00001111); break;
	}
}