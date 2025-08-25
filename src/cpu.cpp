#include "cpu.h"
#include "bus.h"
#include <stdexcept>

constexpr CPU::Operand R8(int r)		{ return { CPU::OperandType::R8, r }; };
constexpr CPU::Operand R16(int r)		{ return { CPU::OperandType::R16, r }; };
constexpr CPU::Operand R16STK(int r)	{ return { CPU::OperandType::R16STK, r }; };
constexpr CPU::Operand R16MEM(int r)	{ return { CPU::OperandType::R16MEM, r }; };
constexpr CPU::Operand COND(int c)		{ return { CPU::OperandType::COND, c }; };
constexpr CPU::Operand VEC(int v)       { return { CPU::OperandType::VEC, v }; };

CPU::CPU() {
	constexpr Operand empt = { OperandType::NONE, 0 };
	constexpr Operand n8 = { OperandType::n8, 0 };
	constexpr Operand n16 = { OperandType::n16, 0 };
	constexpr Operand a8 = { OperandType::a8, 0 };
	constexpr Operand a16 = { OperandType::a16, 0 };

	lookup =
	{
	//   MNEMONIC        C  FUNC  SRC            DST                MNEMONIC      C  FUNC  SRC          DST                MNEMONIC       C  FUNC SRC              DST                MNEMONIC      C  FUNC SRC          DST                MNEMONIC        C  FUNC  SRC              DST                MNEMONIC      C  FUNC  SRC          DST                MNEMONIC       C  FUNC  SRC              DST                MNEMONIC      C  FUNC  SRC        DST
		{"NOP",          1, NOP,  empt,          empt},            {"LD BC, n16", 3, LD,   n16,         R16(REG_BC)},     {"LD [BC], A",  2, LD,  R8(REG_A),       R16MEM(REG_BC)},  {"INC BC",     2, INC, R16(REG_BC), empt},            {"INC B",        1, INC,  R8(REG_B),       empt},            {"DEC B",      1, DEC,  R8(REG_B),   empt},            {"LD B, n8",    2, LD,   n8,              R8(REG_B)},       {"RLCA",       1, RLCA, empt,      empt},
		{"LD a16, SP",   5, LD,   R16(REG_SP),   a16},             {"ADD HL, BC", 2, ADD,  R16(REG_BC), R16(REG_HL)},     {"LD A, [BC]",  2, LD,  R16MEM(REG_BC),  R8(REG_A)},       {"DEC BC",     2, DEC, R16(REG_BC), empt},            {"INC C",        1, INC,  R8(REG_C),       empt},            {"DEC C",      1, DEC,  R8(REG_C),   empt},            {"LD C, n8",    2, LD,   n8,              R8(REG_C)},       {"RRCA",       1, RRCA, empt,      empt},
		{"STOP n8",	     1, STOP, n8,            empt},            {"LD DE, n16", 3, LD,   n16,         R16(REG_DE)},     {"LD [DE], A",  2, LD,  R8(REG_A),       R16MEM(REG_DE)},  {"INC DE",     2, INC, R16(REG_DE), empt},            {"INC D",        1, INC,  R8(REG_D),       empt},            {"DEC D",      1, DEC,  R8(REG_D),   empt},            {"LD D, n8",    2, LD,   n8,              R8(REG_D)},       {"RLA",        1, RLA,  empt,      empt},
		{"JR n8",        2, JR,   n8,            empt},            {"ADD HL, DE", 2, ADD,  R16(REG_DE), R16(REG_HL)},     {"LD A, [DE]",  2, LD,  R16MEM(REG_DE),  R8(REG_A)},       {"DEC DE",     2, DEC, R16(REG_DE), empt},            {"INC E",        1, INC,  R8(REG_E),       empt},            {"DEC E",      1, DEC,  R8(REG_E),   empt},            {"LD E, n8",    2, LD,   n8,              R8(REG_E)},       {"RRA",        1, RRA,  empt,      empt},
		{"JR NZ, n8",    2, JR,   n8,            COND(COND_NZ)},   {"LD HL, n16", 2, LD,   n16,         R16(REG_HL)},     {"LD [HL+], A", 2, LD,  R8(REG_A),       R16MEM(REG_HLI)}, {"INC HL",     2, INC, R16(REG_HL), empt},            {"INC H",        1, INC,  R8(REG_H),       empt},            {"DEC H",      1, DEC,  R8(REG_H),   empt},            {"LD H, n8",    2, LD,   n8,              R8(REG_H)},       {"DAA",        1, DAA,  empt,      empt},
		{"JR Z, n8",     2, JR,   n8,            COND(COND_Z)},    {"ADD HL, HL", 2, ADD,  R16(REG_HL), R16(REG_HL)},     {"LD A, [HL+]", 2, LD,  R16MEM(REG_HLI), R8(REG_A)},       {"DEC HL",     2, DEC, R16(REG_HL), empt},            {"INC L",        1, INC,  R8(REG_L),       empt},            {"DEC L",      1, DEC,  R8(REG_L),   empt},            {"LD L, n8",    2, LD,   n8,              R8(REG_L)},       {"CPL",        1, CPL,  empt,      empt},
		{"JR NC, n8",    2, JR,   n8,            COND(COND_NC)},   {"LD SP, n16", 2, LD,   n16,         R16(REG_SP)},     {"LD [HL-], A", 2, LD,  R8(REG_A),       R16MEM(REG_HLD)}, {"INC SP",     2, INC, R16(REG_SP), empt},            {"INC [HL]",     3, INC,  R8(REG_HL_DATA), empt},            {"DEC [HL]",   3, DEC,  R8(REG_H),   empt},            {"LD [HL], n8", 3, LD,   n8,              R8(REG_HL_DATA)}, {"SCF",        1, SCF,  empt,      empt},
		{"JR C, n8",     2, JR,   n8,            COND(COND_C)},    {"ADD HL, SP", 2, ADD,  R16(REG_SP), R16(REG_HL)},     {"LD A, [HL-]", 2, LD,  R16MEM(REG_HLD), R8(REG_A)},       {"DEC SP",     2, DEC, R16(REG_SP), empt},            {"INC A",        1, INC,  R8(REG_A),       empt},            {"DEC A",      1, DEC,  R8(REG_L),   empt},            {"LD A, n8",    2, LD,   n8,              R8(REG_A)},       {"CCF",        1, CCF,  empt,      empt},

		{"LD B, B",      1, LD,   R8(REG_B),     R8(REG_B)},       {"LD B, C",    1, LD,   R8(REG_C),   R8(REG_B)},       {"LD B, D",     1, LD,  R8(REG_D),       R8(REG_B)},       {"LD B, E",    1, LD,  R8(REG_E),   R8(REG_B)},       {"LD B, H",      1, LD,   R8(REG_H),       R8(REG_B)},       {"LD B, L",    1, LD,   R8(REG_L),   R8(REG_B)},       {"LD B, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_B)},       {"LD B, A",    1, LD,   R8(REG_A), R8(REG_B)},
		{"LD C, B",      1, LD,   R8(REG_B),     R8(REG_C)},       {"LD C, C",    1, LD,   R8(REG_C),   R8(REG_C)},       {"LD C, D",     1, LD,  R8(REG_D),       R8(REG_C)},       {"LD C, E",    1, LD,  R8(REG_E),   R8(REG_C)},       {"LD C, H",      1, LD,   R8(REG_H),       R8(REG_C)},       {"LD C, L",    1, LD,   R8(REG_L),   R8(REG_C)},       {"LD C, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_C)},       {"LD C, A",    1, LD,   R8(REG_A), R8(REG_C)},
		{"LD D, B",      1, LD,   R8(REG_B),     R8(REG_D)},       {"LD D, C",    1, LD,   R8(REG_C),   R8(REG_D)},       {"LD D, D",     1, LD,  R8(REG_D),       R8(REG_D)},       {"LD D, E",    1, LD,  R8(REG_E),   R8(REG_D)},       {"LD D, H",      1, LD,   R8(REG_H),       R8(REG_D)},       {"LD D, L",    1, LD,   R8(REG_L),   R8(REG_D)},       {"LD D, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_D)},       {"LD D, A",    1, LD,   R8(REG_A), R8(REG_D)},
		{"LD E, B",      1, LD,   R8(REG_B),     R8(REG_E)},       {"LD E, C",    1, LD,   R8(REG_C),   R8(REG_E)},       {"LD E, D",     1, LD,  R8(REG_D),       R8(REG_E)},       {"LD E, E",    1, LD,  R8(REG_E),   R8(REG_E)},       {"LD E, H",      1, LD,   R8(REG_H),       R8(REG_E)},       {"LD E, L",    1, LD,   R8(REG_L),   R8(REG_E)},       {"LD E, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_E)},       {"LD E, A",    1, LD,   R8(REG_A), R8(REG_E)},
		{"LD H, B",      1, LD,   R8(REG_B),     R8(REG_H)},       {"LD H, C",    1, LD,   R8(REG_C),   R8(REG_H)},       {"LD H, D",     1, LD,  R8(REG_D),       R8(REG_H)},       {"LD H, E",    1, LD,  R8(REG_E),   R8(REG_H)},       {"LD H, H",      1, LD,   R8(REG_H),       R8(REG_H)},       {"LD H, L",    1, LD,   R8(REG_L),   R8(REG_H)},       {"LD H, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_H)},       {"LD H, A",    1, LD,   R8(REG_A), R8(REG_H)},
		{"LD L, B",      1, LD,   R8(REG_B),     R8(REG_L)},       {"LD L, C",    1, LD,   R8(REG_C),   R8(REG_L)},       {"LD L, D",     1, LD,  R8(REG_D),       R8(REG_L)},       {"LD L, E",    1, LD,  R8(REG_E),   R8(REG_L)},       {"LD L, H",      1, LD,   R8(REG_H),       R8(REG_L)},       {"LD L, L",    1, LD,   R8(REG_L),   R8(REG_L)},       {"LD L, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_L)},       {"LD L, A",    1, LD,   R8(REG_A), R8(REG_L)},
		{"LD [HL], B",   2, LD,   R8(REG_B),     R8(REG_HL_DATA)}, {"LD [HL], C", 2, LD,   R8(REG_C),   R8(REG_HL_DATA)}, {"LD [HL], D",  2, LD,  R8(REG_D),       R8(REG_HL_DATA)}, {"LD [HL], E", 2, LD,  R8(REG_E),   R8(REG_HL_DATA)}, {"LD [HL], H",   2, LD,   R8(REG_H),       R8(REG_HL_DATA)}, {"LD [HL], L", 2, LD,   R8(REG_L),   R8(REG_HL_DATA)}, {"HALT",        1, HALT, empt,            empt},            {"LD [HL], A", 1, LD,   R8(REG_A), R8(REG_HL_DATA)},
		{"LD A, B",      1, LD,   R8(REG_B),     R8(REG_A)},       {"LD A, C",    1, LD,   R8(REG_C),   R8(REG_A)},       {"LD A, D",     1, LD,  R8(REG_D),       R8(REG_A)},       {"LD A, E",    1, LD,  R8(REG_E),   R8(REG_A)},       {"LD A, H",      1, LD,   R8(REG_H),       R8(REG_A)},       {"LD A, L",    1, LD,   R8(REG_L),   R8(REG_A)},       {"LD A, [HL]",  2, LD,   R8(REG_HL_DATA), R8(REG_A)},       {"LD A, A",    1, LD,   R8(REG_A), R8(REG_A)},

		{"ADD A, B",     1, ADD,  R8(REG_B),     R8(REG_A)},       {"ADD A, C",   1, ADD,  R8(REG_C),   R8(REG_A)},       {"ADD A, D",    1, ADD, R8(REG_D),       R8(REG_A)},       {"ADD A, E",   1, ADD, R8(REG_E),   R8(REG_A)},       {"ADD A, H",     1, ADD,  R8(REG_H),       R8(REG_A)},       {"ADD A, L",   1, ADD,  R8(REG_L),   R8(REG_A)},       {"ADD A, [HL]", 2, ADD,  R8(REG_HL_DATA), R8(REG_A)},       {"ADD A, A",   1, ADD,  R8(REG_A), R8(REG_A)},
		{"ADC A, B",     1, ADC,  R8(REG_B),     R8(REG_A)},       {"ADC A, C",   1, ADC,  R8(REG_C),   R8(REG_A)},       {"ADC A, D",    1, ADC, R8(REG_D),       R8(REG_A)},       {"ADC A, E",   1, ADC, R8(REG_E),   R8(REG_A)},       {"ADC A, H",     1, ADC,  R8(REG_H),       R8(REG_A)},       {"ADC A, L",   1, ADC,  R8(REG_L),   R8(REG_A)},       {"ADC A, [HL]", 2, ADC,  R8(REG_HL_DATA), R8(REG_A)},       {"ADC A, A",   1, ADC,  R8(REG_A), R8(REG_A)},
		{"SUB A, B",     1, SUB,  R8(REG_B),     R8(REG_A)},       {"SUB A, C",   1, SUB,  R8(REG_C),   R8(REG_A)},       {"SUB A, D",    1, SUB, R8(REG_D),       R8(REG_A)},       {"SUB A, E",   1, SUB, R8(REG_E),   R8(REG_A)},       {"SUB A, H",     1, SUB,  R8(REG_H),       R8(REG_A)},       {"SUB A, L",   1, SUB,  R8(REG_L),   R8(REG_A)},       {"SUB A, [HL]", 2, SUB,  R8(REG_HL_DATA), R8(REG_A)},       {"SUB A, A",   1, SUB,  R8(REG_A), R8(REG_A)},
		{"SBC A, B",     1, SBC,  R8(REG_B),     R8(REG_A)},       {"SBC A, C",   1, SBC,  R8(REG_C),   R8(REG_A)},       {"SBC A, D",    1, SBC, R8(REG_D),       R8(REG_A)},       {"SBC A, E",   1, SBC, R8(REG_E),   R8(REG_A)},       {"SBC A, H",     1, SBC,  R8(REG_H),       R8(REG_A)},       {"SBC A, L",   1, SBC,  R8(REG_L),   R8(REG_A)},       {"SBC A, [HL]", 2, SBC,  R8(REG_HL_DATA), R8(REG_A)},       {"SBC A, A",   1, SBC,  R8(REG_A), R8(REG_A)},
		{"AND A, B",     1, AND,  R8(REG_B),     R8(REG_A)},       {"AND A, C",   1, AND,  R8(REG_C),   R8(REG_A)},       {"AND A, D",    1, AND, R8(REG_D),       R8(REG_A)},       {"AND A, E",   1, AND, R8(REG_E),   R8(REG_A)},       {"AND A, H",     1, AND,  R8(REG_H),       R8(REG_A)},       {"AND A, L",   1, AND,  R8(REG_L),   R8(REG_A)},       {"AND A, [HL]", 2, AND,  R8(REG_HL_DATA), R8(REG_A)},       {"AND A, A",   1, AND,  R8(REG_A), R8(REG_A)},
		{"XOR A, B",     1, XOR,  R8(REG_B),     R8(REG_A)},       {"XOR A, C",   1, XOR,  R8(REG_C),   R8(REG_A)},       {"XOR A, D",    1, XOR, R8(REG_D),       R8(REG_A)},       {"XOR A, E",   1, XOR, R8(REG_E),   R8(REG_A)},       {"XOR A, H",     1, XOR,  R8(REG_H),       R8(REG_A)},       {"XOR A, L",   1, XOR,  R8(REG_L),   R8(REG_A)},       {"XOR A, [HL]", 2, XOR,  R8(REG_HL_DATA), R8(REG_A)},       {"XOR A, A",   1, XOR,  R8(REG_A), R8(REG_A)},
		{"OR A, B",      1, OR,   R8(REG_B),     R8(REG_A)},       {"OR A, C",    1, OR,   R8(REG_C),   R8(REG_A)},       {"OR A, D",     1, OR,  R8(REG_D),       R8(REG_A)},       {"OR A, E",    1, OR,  R8(REG_E),   R8(REG_A)},       {"OR A, H",      1, OR,   R8(REG_H),       R8(REG_A)},       {"OR A, L",    1, OR,   R8(REG_L),   R8(REG_A)},       {"OR A, [HL]",  2, OR,   R8(REG_HL_DATA), R8(REG_A)},       {"OR A, A",    1, OR,   R8(REG_A), R8(REG_A)},
		{"CP A, B",      1, CP,   R8(REG_B),     R8(REG_A)},       {"CP A, C",    1, CP,   R8(REG_C),   R8(REG_A)},       {"CP A, D",     1, CP,  R8(REG_D),       R8(REG_A)},       {"CP A, E",    1, CP,  R8(REG_E),   R8(REG_A)},       {"CP A, H",      1, CP,   R8(REG_H),       R8(REG_A)},       {"CP A, L",    1, CP,   R8(REG_L),   R8(REG_A)},       {"CP A, [HL]",  2, CP,   R8(REG_HL_DATA), R8(REG_A)},       {"CP A, A",    1, CP,   R8(REG_A), R8(REG_A)},

		{"RET NZ",       2, RET,  COND(COND_NZ), empt},            {"POP BC",     3, POP,  R16(REG_BC), empt},            {"JP NZ, a16",  3, JP,  a16,             COND(COND_NZ)},   {"JP a16",     4, JP,  a16,         empt},            {"CALL NZ, a16", 3, CALL, a16,             COND(COND_NZ)},   {"PUSH BC",    4, PUSH, R16(REG_BC), empt},            {"ADD A, n8",   2, ADD,  n8,              R8(REG_A)},       {"RST $00",    4, RST,  VEC(0x00), empt},
		{"RET Z",        2, RET,  COND(COND_Z),  empt},            {"RET",        4, RET,  empt,        empt},            {"JP Z, a16",   3, JP,  a16,             COND(COND_Z)},    {"PREFIX",     1, CB,  empt,        empt},            {"CALL Z, a16",  3, CALL, a16,             COND(COND_Z)},    {"CALL a16",   6, CALL, a16,         empt},            {"ADC A, n8",   2, ADC,  n8,              R8(REG_A)},       {"RST $08",    4, RST,  VEC(0x08), empt},
		{"RET NC",       2, RET,  COND(COND_NC), empt},            {"POP DE",     3, POP,  R16(REG_DE), empt},            {"JP NC, a16",  3, JP,  a16,             COND(COND_NC)},   {"INVALID",    1, INV, empt,        empt},            {"CALL NC, a16", 3, CALL, a16,             COND(COND_NC)},   {"PUSH DE",    4, PUSH, R16(REG_DE), empt},            {"SUB A, n8",   2, SUB,  n8,              R8(REG_A)},       {"RST $10",    4, RST,  VEC(0x10), empt},
		{"RET C",        2, RET,  COND(COND_C),  empt},            {"RETI",       4, RETI, empt,        empt},            {"JP C, a16",   3, JP,  a16,             COND(COND_C)},    {"INVALID",    1, INV, empt,        empt},            {"CALL C, a16",  3, CALL, a16,             COND(COND_C)},    {"INVALID",    1, INV,  empt,        empt},            {"SBC A, n8",   2, SBC,  n8,              R8(REG_A)},       {"RST $18",    4, RST,  VEC(0x18), empt},
		{"LDH a8, A",    3, LDH,  a8,            R8(REG_A)},       {"POP HL",     3, POP,  R16(REG_HL), empt},            {"LDH [C], A",  2, LDH, R8(REG_C),       R8(REG_A)},       {"INVALID",    1, INV, empt,        empt},            {"INVALID",      1, INV,  empt,            empt},            {"PUSH HL",    4, PUSH, R16(REG_HL), empt},            {"AND A, n8",   2, AND,  n8,              R8(REG_A)},       {"RST $20",    4, RST,  VEC(0x20), empt},
		{"ADD SP, e8",   4, ADD,  n8,            R16(REG_SP)},     {"JP HL",      1, JP,   R16(REG_HL), empt},            {"LD a16, A",   4, LD,  a16,             R8(REG_A)},       {"INVALID",    1, INV, empt,        empt},            {"INVALID",      1, INV,  empt,            empt},            {"INVALID",    1, INV,  empt,        empt},            {"XOR A, n8",   2, XOR,  n8,              R8(REG_A)},       {"RST $28",    4, RST,  VEC(0x28), empt},
		{"LDH a8",       3, LDH,  R8(REG_A),     a8},              {"POP AF",     3, POP,  R16(REG_AF), empt},            {"LDH A, [C]",  2, LDH, R8(REG_A),       R8(REG_C)},       {"DI",         1, DI,  empt,        empt},            {"INVALID",      1, INV,  empt,            empt},            {"PUSH AF",    4, PUSH, R16(REG_AF), empt},            {"OR A, n8",    2, OR,   n8,              R8(REG_A)},       {"RST $30",    4, RST,  VEC(0x30), empt},
		{"LD HL, SP+n8", 3, LD,   empt, /*fix*/  empt},            {"LD SP, HL",  2, LD,   R16(REG_HL), R16(REG_SP)},     {"LD A, a16",   4, LD,  a16,             R8(REG_A)},       {"EI",         1, EI,  empt,        empt},            {"INVALID",      1, INV,  empt,            empt},            {"INVALID",    1, INV,  empt,        empt},            {"CP A, n8",    2, CP,   n8,              R8(REG_A)},       {"RST $38",    4, RST,  VEC(0x38), empt},
	};
}

CPU::~CPU() {

}

uint8_t CPU::read(uint16_t addr) {
	return bus->read(addr);
}

void CPU::write(uint16_t addr, uint8_t data) {
	bus->write(addr, data);
}

uint8_t CPU::fetchByte() {
	return bus->read(pc++);
}

uint16_t CPU::fetchWord() {
	uint16_t lo = bus->read(pc++);
	uint16_t hi = bus->read(pc++);
	return (hi << 8) | lo;
}

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

void CPU::decode(uint8_t opcode) {
	
}



void CPU::readOperand(Operand op) {
	switch (op.type) {
	case R8:
	case R16:
	case R16STK:
	case R16MEM:
	case COND:

	case B3:
	case TGT3:
	case IMM8: 
	case IMM16:
	}
}




// Loads
void CPU::LD(Operand src, Operand dst) {
	uint16_t value = readOperand(src);
	writeOperand(dst, value);
}

void CPU::LDH(Operand src, Operand dst) {
	// Not implemented
}

// Arithmetic
void CPU::ADC(Operand src, Operand dst) {

}

void CPU::ADD(Operand src, Operand dst) {

}

void CPU::SBC(Operand src, Operand dst) {

}

void CPU::SUB(Operand src, Operand dst) {

}

void CPU::INC(Operand src, Operand dst) {

}

void CPU::DEC(Operand src, Operand dst) {

}

void CPU::CP(Operand src, Operand dst) {

}

// Bitwise Logic
void CPU::AND(Operand src, Operand dst) {

}

void CPU::CPL(Operand src, Operand dst) {

}

void CPU::OR(Operand src, Operand dst) {

}

void CPU::XOR(Operand src, Operand dst) {

}

// Bit Flag
void CPU::BIT(Operand src, Operand dst) {

}

void CPU::SET(Operand src, Operand dst) {

}

void CPU::RES(Operand src, Operand dst) {

}

// Bit Shift

void CPU::RL(Operand src, Operand dst) {

}

void CPU::RLA(Operand src, Operand dst) {

}

void CPU::RLC(Operand src, Operand dst) {

}

void CPU::RLCA(Operand src, Operand dst) {

}

void CPU::RR(Operand src, Operand dst) {

}

void CPU::RRA(Operand src, Operand dst) {

}

void CPU::RRC(Operand src, Operand dst) {

}

void CPU::RRCA(Operand src, Operand dst) {

}

void CPU::SLA(Operand src, Operand dst) {

}

void CPU::SRA(Operand src, Operand dst) {

}

void CPU::SRL(Operand src, Operand dst) {

}

void CPU::SWAP(Operand src, Operand dst) {

}

// Jumps and Subroutines
void CPU::CALL(Operand src, Operand dst) {

}

void CPU::JP(Operand src, Operand dst) {

}

void CPU::JR(Operand src, Operand dst) {

}

void CPU::RET(Operand src, Operand dst) {

}

void CPU::RETI(Operand src, Operand dst) {

}

void CPU::RST(Operand src, Operand dst) {

}

// Carry Flag
void CPU::CCF(Operand src, Operand dst) {

}

void CPU::SCF(Operand src, Operand dst) {

}

// Stack
void CPU::POP(Operand src, Operand dst) {

}

void CPU::PUSH(Operand src, Operand dst) {

}

// Interrupt-related
void CPU::DI(Operand src, Operand dst) {

}

void CPU::EI(Operand src, Operand dst) {

}

void CPU::HALT(Operand src, Operand dst) {

}

// Miscellaneous
void CPU::DAA(Operand src, Operand dst) {

}

void CPU::NOP(Operand src, Operand dst) {

}

void CPU::STOP(Operand src, Operand dst) {

}

void CPU::CB(Operand src, Operand dst) {

}

void CPU::INV(Operand src, Operand dst) {

}