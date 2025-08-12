#include "cpu.h"
#include <stdexcept>

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
		break;
	case 7:
		return A();
	}
	throw std::runtime_error("Invalid r8 selection");
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
	throw std::runtime_error("Invalid r16 selection");
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
	throw std::runtime_error("Invalid r16stk selection");
}

uint8_t opcode_group(uint8_t opcode) {
	return (opcode | 0b11000000) >> 6;
}

void CPU::decode(uint8_t opcode) {
	switch (opcode_group(opcode))
	{
	case GROUP_0:
		break;
	case GROUP_1:
		break;

	case GROUP_2: {
		uint8_t operand = select_r8(opcode | 0b00000111);
		switch ((opcode | 0b00111000) >> 3) {
		case 0:
			//add
			break;
		case 1:
			//adc
			break;
		case 2:
			//sub
			break;
		case 3:
			//sbc
			break;
		case 4:
			//and
			break;
		case 5:
			//xor
			break;
		case 6:
			//or
			break;
		case 7:
			//cp
			break;
		}
		break;
	}
	case GROUP_3:
		break;

	}
}