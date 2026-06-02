#include "joypad.h"
#include "bus.h"

void Joypad::press(uint8_t key) {
	uint8_t check = 0;
	if (key & BUTTONS) {
		key &= KEY;
		buttons &= ~key;
		check = BUTTON_ENABLE;
	}
	else {
		dpad &= ~key;
		check = DPAD_ENABLE;
	}

	// Only affect joypad if the keyset is enabled
	if (~bus->joyp & check) {
		// If the key is not currently active
		if (bus->joyp & key) {
			bus->joyp &= ~key;
			bus->interrupts |= INTERRUPT_JOYPAD;
		}
	}
}

void Joypad::release(uint8_t key) {
	uint8_t check = 0;
	uint8_t opposite = 0;

	if (key & BUTTONS) {
		key &= KEY;
		buttons |= key;

		// Only affect joypad if the keyset is enabled
		if (~bus->joyp & BUTTON_ENABLE) {
			// if the other keyset is disabled OR (it is enabled AND the other button is not active) then reset the key
			if ((bus->joyp & DPAD_ENABLE) || ((~bus->joyp & DPAD_ENABLE) && (dpad & key))) {
				bus->joyp |= key;
			}
		}
	}

	else {
		dpad |= key;

		// Only affect joypad if the keyset is enabled
			if (~bus->joyp & DPAD_ENABLE) {
				// if the other keyset is disabled OR (it is enabled AND the other button is not active) then reset the key
				if ((bus->joyp & BUTTON_ENABLE) || ((~bus->joyp & BUTTON_ENABLE) && (buttons & key))) {
					bus->joyp |= key;
				}
			}
	}
}