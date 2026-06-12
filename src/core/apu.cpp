#include "apu.h"
#include "bus.h"

#define FULL_TIMER		0b00111111
#define TRIGGER			0b10000000
#define DUTY_OVERFLOW	0x800

void APU::initialize() {
	speaker.initialize(this);
	shutoff();
}

void APU::shutoff() {
	// Reset all audio registers
	nr10 = 0;
	nr11 = 0;
	nr12 = 0;
	nr13 = 0;
	nr14 = 0;
	nr21 = 0;
	nr22 = 0;
	nr23 = 0;
	nr24 = 0;
	nr30 = 0;
	nr31 = 0;
	nr32 = 0;
	nr33 = 0;
	nr34 = 0;
	nr41 = 0;
	nr42 = 0;
	nr43 = 0;
	nr44 = 0;
	nr50 = 0;
	nr51 = 0;

	// Disable all DACs
	ch1.dac_enable = false;
	ch2.dac_enable = false;
	//ch3.dac_enable = false;
	ch4.dac_enable = false;

	// Disable all channels
	nr52 &= 0b11110000;
}

void APU::clock() {
	// Increment period timer every 4 T-cycles
	// When period overflows, reset contents
	// and advance duty position

	// Period divider
	if (bus->master_clock % 4 == 0) {
		// Channel 1
		ch1.period_value++;
		
		if (ch1.period_value == DUTY_OVERFLOW) {
			// Reset period value
			ch1.period_value = nr14 & CH1_PRD_HIGH;
			ch1.period_value <<= 8;
			ch1.period_value |= nr13;
			
			// Advance duty position
			ch1.duty_position++;
			ch1.duty_position &= 7;

			// Find new duty value
			ch1.duty_value = duty_table[nr11 >> 6][ch1.duty_position];
		}

		// Channel 2
		ch2.period_value++;

		if (ch2.period_value == DUTY_OVERFLOW) {
			// Reset period value
			ch2.period_value = nr24 & CH2_PRD_HIGH;
			ch2.period_value <<= 8;
			ch2.period_value |= nr23;

			// Advance duty position
			ch2.duty_position++;
			ch2.duty_position &= 7;

			// Find new duty value
			ch2.duty_value = duty_table[nr21 >> 6][ch2.duty_position];
		}

		// Channel 4
		if (bus->master_clock % 16 == 0 &&
			!(ch4.shift == 14 || ch4.shift == 15)) {

			ch4.shift_timer++;

			uint8_t scale = 1;

			uint16_t divide = 1;
			if (ch4.shift != 0) {
				divide <<= (ch4.shift - 1);
			}
				
			if (ch4.divider == 0) {
				scale = 2;
			}
			else {
				divide *= ch4.divider;
			}

			if ((ch4.shift_timer * scale) % divide == 0) {
				ch4.shift_timer = 0;
				bool bit = (ch4.lsfr & 0b1) == ((ch4.lsfr >> 1) & 0b1);
				if (bit) {
					ch4.lsfr |= CH4_LONG_BIT;

					if (nr43 & CH4_SHORT_LSFR) {
						ch4.lsfr |= CH4_SHORT_BIT;
					}
				}
				else {
					ch4.lsfr &= ~CH4_LONG_BIT;

					if (nr43 & CH4_SHORT_LSFR) {
						ch4.lsfr &= ~CH4_SHORT_BIT;
					}
				}

				ch4.lsfr_value = ch4.lsfr & 1;
				ch4.lsfr >>= 1;
			}
		}
	}

	cycle_accumulator++;
	if (cycle_accumulator >= cyclesPerSample) {
		cycle_accumulator -= cyclesPerSample;
		speaker.sample();
	}

	if (speaker.samples.size() >= 512) {
		speaker.flush();
	}

	if (bus->div & 0b00011111) {
		no_tick = false;
		return;
	}
	
	if (no_tick) {
		return;
	}

	// 512 Hz
	div++;
	no_tick = true;

	speaker.adjustRate();

	// Sound Length - 256Hz
	if (div % 2 == 0) {
		// If Channel 1 Length Enabled
		if ((nr14 & CH1_TMR_ENA) && (ch1.len_timer < CH1_TMR_DIS)) {
			if (++ch1.len_timer == CH1_TMR_DIS) {
				nr52 &= CH1_OFF;
			}
		}

		// If Channel 2 Length Enabled
		if ((nr24 & CH2_TMR_ENA) && (ch2.len_timer < CH2_TMR_DIS)) {
			if (++ch2.len_timer == CH2_TMR_DIS) {
				nr52 &= CH2_OFF;
			}
		}

		// If Channel 4 Length Enabled
		if ((nr44 & CH4_TMR_ENA) && (ch4.len_timer < CH4_TMR_DIS)) {
			if (++ch4.len_timer == CH4_TMR_DIS) {
				nr52 &= CH4_OFF;
			}
		}
	}
	
	// CH1 Freq Sweep - 128Hz
	if (div % 4 == 0) {
		// Make sure this turns off channel!
	}

	// Envelope Sweep - 64Hz
	if (div % 8 == 0) {
		// Channel 1 Sweep
		ch1.env_sweep_timer++;
		if (nr12 & CH1_SWP_PACE) {
			if (ch1.env_sweep_timer % (nr12 & CH1_SWP_PACE) == 0) {
				if (nr12 & CH1_ENV_DIR) {
					if (ch1.volume != 0xF) {
						ch1.volume++;
					}
				}
				else {
					if (ch1.volume != 0) {
						ch1.volume--;
					}
				}
			}
		}
		// Channel 2 Sweep
 		ch2.env_sweep_timer++;
		if (nr22 & CH2_SWP_PACE) {
			if (ch2.env_sweep_timer % (nr22 & CH2_SWP_PACE) == 0) {
				if (nr22 & CH2_ENV_DIR) {
					if (ch2.volume != 0xF) {
						ch2.volume++;
					}
				}
				else {
					if (ch2.volume != 0) {
						ch2.volume--;
					}
				}
			}
		}
		// Channel 3 Sweep

		// Channel 4 Sweep
		ch4.env_sweep_timer++;
		if (nr42 & CH4_SWP_PACE) {
			if (ch4.env_sweep_timer % (nr42 & CH4_SWP_PACE) == 0) {
				if (nr42 & CH4_ENV_DIR) {
					if (ch4.volume != 0xF) {
						ch4.volume++;
					}
				}
				else {
					if (ch4.volume != 0) {
						ch4.volume--;
					}
				}
			}
		}
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
	if ((!(nr52 & 0b10000000)) && (addr != 0xFF26)) {
		return;
	}

	switch (addr) {
	// Channel 1
	case 0xFF10: // Sweep
		nr10 = data & 0b01111111; break;
	case 0xFF11: // Length Timer & Duty Cycle
		nr11 = data; break;
	case 0xFF12: // Volume & Envelope
		nr12 = data;
		ch1.dac_enable = ((nr12 & 0xF8) != 0);
		if (!ch1.dac_enable) {
			nr52 &= CH1_OFF;
		}
		break;
	case 0xFF13: // Period Low
		nr13 = data; break;
	case 0xFF14: // Period High & Control
		nr14 = data & 0b11000111;
		if (data & TRIGGER) {
			//Sweep does several things. - ???

			// Ch1 is enabled.
			if (ch1.dac_enable) {
				nr52 |= CH1_ON;
			}

			// If length timer expired it is reset.
			if (ch1.len_timer == CH1_TMR_DIS) {
				ch1.len_timer = (nr11 & CH1_TMR_INIT);
			}

			//The period divider is set to the contents of NR13 and NR14.
			ch1.period_value = nr14 & CH1_PRD_HIGH;
			ch1.period_value <<= 8;
			ch1.period_value |= nr13;

			// Volume is set to contents of NR12 initial volume.
			ch1.volume = (nr12 >> 4);

			// Envelope timer is reset.
			ch1.env_sweep_timer = 0;
		}
		break;

	// Channel 2
	case 0xFF16: // Length Timer & Duty Cycle
		nr21 = data; break;
	case 0xFF17: // Volume & Envelope
		nr22 = data;
		ch2.dac_enable = ((nr22 & 0xF8) != 0);
		if (!ch2.dac_enable) {
			nr52 &= CH2_OFF;
		}
		break;
	case 0xFF18: // Period Low
		nr23 = data; break;
	case 0xFF19: // Period High & Control
		nr24 = data & 0b11000111;
		if (data & TRIGGER) {
			// Ch2 is enabled.
			if (ch2.dac_enable) {
				nr52 |= CH2_ON;
			}

			// If length timer expired it is reset.
			if (ch2.len_timer == CH2_TMR_DIS) {
				ch2.len_timer = (nr21 & CH2_TMR_INIT);
			}

			//The period divider is set to the contents of NR13 and NR14.
			ch2.period_value = nr24 & CH2_PRD_HIGH;
			ch2.period_value <<= 8;
			ch2.period_value |= nr23;

			// Volume is set to contents of NR12 initial volume.
			ch2.volume = (nr22 >> 4);

			// Envelope timer is reset.
			ch2.env_sweep_timer = 0;
		}
		break;

	// Channel 3
	case 0xFF1A: nr30 = data & 0b10000000; break;
	case 0xFF1B: nr31 = data; break;
	case 0xFF1C: nr32 = data & 0b01100000; break;
	case 0xFF1D: nr33 = data; break;
	case 0xFF1E: nr34 = data & 0b11000111; break;

	// Channel 4
	case 0xFF20: // Length Timer
		nr41 = data & 0b00111111; break;
	case 0xFF21: // Volume & Envelope
		nr42 = data;
		ch4.dac_enable = ((nr42 & 0xF8) != 0);
		if (!ch4.dac_enable) {
			nr52 &= CH4_OFF;
		}
		break;
	case 0xFF22: // Frequency & Randomness
		nr43 = data;
		ch4.divider = nr43 & 0b111;
		ch4.shift = nr43 >> 4;
		break;
	case 0xFF23: // CH4 Control
		nr44 = data & 0b11000000;
		
		if (data & TRIGGER) {
			// Ch4 is enabled.
			if (ch4.dac_enable) {
				nr52 |= CH4_ON;
			}

			// If length timer expired it is reset.
			if (ch4.len_timer == CH4_TMR_DIS) {
				ch4.len_timer = (nr41 & CH4_TMR_INIT);
			}

			// Envelope timer is reset.
			ch4.env_sweep_timer = 0;

			// Volume is set to contents of NR12 initial volume.
			ch4.volume = (nr42 >> 4);

			// LSFR bits are reset
			ch4.lsfr = 0;
		}
		break;

	// Global Control
	case 0xFF24: nr50 = data; break;
	case 0xFF25: nr51 = data; break;
	case 0xFF26: // Audio Master Control
		// TODO: When APU is written off, all registers are set as 0 and read-only (this turns off all dacs!)
		nr52 = (data & 0b10000000) | (nr52 & 0b00001111);
		if (!(data & 0b10000000)) {
			shutoff();
		}
		break;
	}
}