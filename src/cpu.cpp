#include "cpu.h"
#include <stdexcept>

CPU::CPU() {
}

CPU::~CPU() {

}

constexpr int GROUP_0 = 0b00;
constexpr int GROUP_1 = 0b01;
constexpr int GROUP_2 = 0b10;
constexpr int GROUP_3 = 0b11;

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
	return (opcode & 0b11000000) >> 6;
}

uint8_t first_octal(uint8_t opcode) {
	return (opcode >> 3) & 0b111;
}

uint8_t second_octal(uint8_t opcode) {
	return opcode & 0b111;
}

void CPU::decode(uint8_t opcode) {
	switch (opcode_group(opcode)) {
	case GROUP_0:
		switch (second_octal(opcode)) {
		case 0:
			switch (first_octal(opcode)) {
			case 0:
				// nop
				break;
			case 1:
				// ld [imm16], sp
				break;
			case 2:
				// stop
				break;
			case 3:
				// jr imm8
				break;
			default:
				// jr cond,imm8	0	0	1	(cond)	0	0	0
			}
			break;
		case 1:
			if (opcode & 0b00001000) {
				// add hl, r16
			}
			else {
				// ld r16, imm16
			}
			break;
		case 2:
			if (opcode & 0b00001000) {
				// ld a, [r16mem]	0	0	Source (r16mem)	1	0	1	0
			}
			else {
				// ld [r16mem], a	0	0	Dest (r16mem)	0	0	1	0
			}
			break;
		case 3:
			if (opcode & 0b00001000) {
				// dec r16	0	0	Operand(r16)	1	0	1	1
			}
			else {
				// inc r16	0	0	Operand (r16)	0	0	1	1
			}
			break;
		case 4:
			// inc r8	0	0	Operand (r8)	1	0	0
			break;
		case 5:
			// dec r8	0	0	Operand (r8)	1	0	1
			break;
		case 6:
			// ld r8, imm8	0	0	Dest (r8)	1	1	0
			break;
		case 7:
			switch (first_octal(opcode)) {
			case 0:
				// rlca
				break;
			case 1:
				// rrca
				break;
			case 2:
				// rla
				break;
			case 3:
				// rra
				break;
			case 4:
				// daa
				break;
			case 5:
				// cpl
				break;
			case 6:
				// scf
				break;
			case 7:
				// ccf
				break;
			}
			break;
		}
		break;


	// 8-bit regsiter-to-register loads
	case GROUP_1:
		if (opcode == 0b01110110) {
			//halt
		}
		else {
			// ld r8, r8
			uint8_t destination = select_r8(first_octal(opcode));
			uint8_t source = select_r8(second_octal(opcode));
			destination = source;
		}
		break;


	// 8-bit arithmetic
	case GROUP_2: {
		// ___ a, r8
		uint8_t operand = select_r8(second_octal(opcode));
		switch (first_octal(opcode)) {
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
		switch (second_octal(opcode)) {
		case 0:
			switch (first_octal(opcode)) {
			case 4:
				// ldh [imm8], a
				break;
			case 5:
				// add sp, imm8
				break;
			case 6:
				// ldh a, [imm8]
				break;
			case 7:
				// ld hl, sp + imm8
				break;
			default:
				// ret cond			1	1	0	(cond)	0	0	0
			}
			break;
		case 1:
			switch (first_octal(opcode)) {
			case 1:
				// ret
				break;
			case 3:
				// reti
				break;
			case 5:
				// jp hl
				break;
			case 7:
				// ld sp, hl
				break;
			default:
				// pop r16stk	1	1  (r16stk)	0	0	0	1
			}
			break;
		case 2:
			switch (first_octal(opcode)) {
			case 4:
				// ldh [c], a
				break;
			case 5:
				// ld [imm16], a
				break;
			case 6:
				// ldh a, [c]
				break;
			case 7:
				// ld a, [imm16]
				break;
			default:
				// jp cond, imm16	1	1	0	(cond)	0	1	0
			}
			break;
		case 3:
			switch (first_octal(opcode)) {
			case 0:
				// jp imm16
				break;
			case 1:
				// $CB prefix
				break;
			case 6:
				// di
				break;
			case 7:
				// ei
				break;
			default:
				// ELSE INVALID OPCODE
			}
			break;
		case 4:
			// call cond, imm16	1	1	0	(cond)	1	0	0
			// ELSE INVALID OPCODE
			break;
		case 5:
			// call imm16	1	1	0	0	1	1	0	1
			// push r16stk	1	1  (r16stk)	0	1	0	1
			break;
		case 6:
			switch (first_octal(opcode)) {
			case 0:
				// add a, imm8
				break;
			case 1:
				// adc a, imm8
				break;
			case 2:
				// sub a, imm8
				break;
			case 3:
				// sbc a, imm8
				break;
			case 4:
				// and a, imm8
				break;
			case 5:
				// xor a, imm8
				break;
			case 6:
				// or a, imm8
				break;
			case 7:
				// cp a, imm8
				break;
			}
			break;
		case 7:
			// rst tgt3	1	1	(tgt3)	1	1	1
			break;
		}

	}
}