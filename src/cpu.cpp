#include "cpu.h"

CPU::CPU() {
}

CPU::~CPU() {

}

#define GROUP_0	0b00
#define GROUP_1	0b01
#define GROUP_2	0b10
#define GROUP_3	0b11

uint8_t& CPU::select_r8(uint8_t selector) {
	switch (selector) {
	case 0:
		return B();
	case 1:
		return C();
	case 2:
		return D();
	case 3:
		return E();
	case 4:
		return H();
	case 5:
		return L();
	case 6:
		// NOT IMPLEMENTED YET, FETCH BYTE AT [HL]
	case 7:
		return A();
	}
}

uint16_t& CPU::select_r16(uint8_t selector) {
	switch (selector) {
	case 0:
		return BC();
	case 1:
		return DE();
	case 2:
		return HL();
	case 3:
		return SP();
	}
}

uint16_t& CPU::select_r16stk(uint8_t selector) {
	switch (selector) {
	case 0:
		return BC();
	case 1:
		return DE();
	case 2:
		return HL();
	case 3:
		return AF();
	}
}

uint8_t opcode_group(uint8_t opcode) {
	return (opcode | 0b11000000) >> 6;
}

void CPU::decode(uint8_t opcode) {
	switch (opcode_group(opcode))
	{
	case GROUP_0:
	case GROUP_1:

	case GROUP_2: {
		uint8_t operand = select_r8(opcode | 0b00000111);
		switch ((opcode | 0b00111000) >> 3) {
		case 0:
			//add
		case 1:
			//adc
		case 2:
			//sub
		case 3:
			//sbc
		case 4:
			//and
		case 5:
			//xor
		case 6:
			//or
		case 7:
			//cp
		}

	}
	case GROUP_3:

	}
}