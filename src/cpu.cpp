#include "cpu.h"
#include "bus.h"
#include <stdexcept>

constexpr CPU::Operand R8(int r)		{ return { CPU::OperandType::R8, r }; };
constexpr CPU::Operand R16(int r)		{ return { CPU::OperandType::R16, r }; };
constexpr CPU::Operand R16STK(int r)	{ return { CPU::OperandType::R16STK, r }; };
constexpr CPU::Operand R16MEM(int r)	{ return { CPU::OperandType::R16MEM, r }; };
constexpr CPU::Operand COND(int c)		{ return { CPU::OperandType::COND, c }; };
constexpr CPU::Operand VEC(int v)       { return { CPU::OperandType::VEC, v }; };
constexpr CPU::Operand B3(int b)		{ return { CPU::OperandType::B3, b }; };

CPU::CPU() {
	constexpr Operand empt = { OperandType::NONE, 0 };
	constexpr Operand n8 = { OperandType::n8, 0 };
	constexpr Operand n16 = { OperandType::n16, 0 };
	constexpr Operand a8 = { OperandType::a8, 0 };
	constexpr Operand a16 = { OperandType::a16, 0 };

	using c = CPU;

	opcode_lookup = 
	{
	//   MNEMONIC        C  FUNC      SRC            DST                MNEMONIC      C  FUNC      SRC          DST                MNEMONIC       C  FUNC     SRC              DST                MNEMONIC      C  FUNC     SRC          DST                MNEMONIC        C  FUNC      SRC              DST                MNEMONIC      C  FUNC      SRC          DST                MNEMONIC       C  FUNC      SRC              DST                MNEMONIC      C  FUNC      SRC        DST
		{"NOP",          1, &c::NOP,  empt,          empt},            {"LD BC, n16", 3, &c::LD,   n16,         R16(REG_BC)},     {"LD [BC], A",  2, &c::LD,  R8(REG_A),       R16MEM(REG_BC)},  {"INC BC",     2, &c::INC, R16(REG_BC), empt},            {"INC B",        1, &c::INC,  R8(REG_B),       empt},            {"DEC B",      1, &c::DEC,  R8(REG_B),   empt},            {"LD B, n8",    2, &c::LD,   n8,              R8(REG_B)},       {"RLCA",       1, &c::RLCA, empt,      empt},
		{"LD a16, SP",   5, &c::LD,   R16(REG_SP),   a16},             {"ADD HL, BC", 2, &c::ADD,  R16(REG_BC), R16(REG_HL)},     {"LD A, [BC]",  2, &c::LD,  R16MEM(REG_BC),  R8(REG_A)},       {"DEC BC",     2, &c::DEC, R16(REG_BC), empt},            {"INC C",        1, &c::INC,  R8(REG_C),       empt},            {"DEC C",      1, &c::DEC,  R8(REG_C),   empt},            {"LD C, n8",    2, &c::LD,   n8,              R8(REG_C)},       {"RRCA",       1, &c::RRCA, empt,      empt},
		{"STOP n8",	     1, &c::STOP, n8,            empt},            {"LD DE, n16", 3, &c::LD,   n16,         R16(REG_DE)},     {"LD [DE], A",  2, &c::LD,  R8(REG_A),       R16MEM(REG_DE)},  {"INC DE",     2, &c::INC, R16(REG_DE), empt},            {"INC D",        1, &c::INC,  R8(REG_D),       empt},            {"DEC D",      1, &c::DEC,  R8(REG_D),   empt},            {"LD D, n8",    2, &c::LD,   n8,              R8(REG_D)},       {"RLA",        1, &c::RLA,  empt,      empt},
		{"JR n8",        2, &c::JR,   n8,            empt},            {"ADD HL, DE", 2, &c::ADD,  R16(REG_DE), R16(REG_HL)},     {"LD A, [DE]",  2, &c::LD,  R16MEM(REG_DE),  R8(REG_A)},       {"DEC DE",     2, &c::DEC, R16(REG_DE), empt},            {"INC E",        1, &c::INC,  R8(REG_E),       empt},            {"DEC E",      1, &c::DEC,  R8(REG_E),   empt},            {"LD E, n8",    2, &c::LD,   n8,              R8(REG_E)},       {"RRA",        1, &c::RRA,  empt,      empt},
		{"JR NZ, n8",    2, &c::JR,   n8,            COND(COND_NZ)},   {"LD HL, n16", 2, &c::LD,   n16,         R16(REG_HL)},     {"LD [HL+], A", 2, &c::LD,  R8(REG_A),       R16MEM(REG_HLI)}, {"INC HL",     2, &c::INC, R16(REG_HL), empt},            {"INC H",        1, &c::INC,  R8(REG_H),       empt},            {"DEC H",      1, &c::DEC,  R8(REG_H),   empt},            {"LD H, n8",    2, &c::LD,   n8,              R8(REG_H)},       {"DAA",        1, &c::DAA,  empt,      empt},
		{"JR Z, n8",     2, &c::JR,   n8,            COND(COND_Z)},    {"ADD HL, HL", 2, &c::ADD,  R16(REG_HL), R16(REG_HL)},     {"LD A, [HL+]", 2, &c::LD,  R16MEM(REG_HLI), R8(REG_A)},       {"DEC HL",     2, &c::DEC, R16(REG_HL), empt},            {"INC L",        1, &c::INC,  R8(REG_L),       empt},            {"DEC L",      1, &c::DEC,  R8(REG_L),   empt},            {"LD L, n8",    2, &c::LD,   n8,              R8(REG_L)},       {"CPL",        1, &c::CPL,  empt,      empt},
		{"JR NC, n8",    2, &c::JR,   n8,            COND(COND_NC)},   {"LD SP, n16", 2, &c::LD,   n16,         R16(REG_SP)},     {"LD [HL-], A", 2, &c::LD,  R8(REG_A),       R16MEM(REG_HLD)}, {"INC SP",     2, &c::INC, R16(REG_SP), empt},            {"INC [HL]",     3, &c::INC,  R8(REG_HL_DATA), empt},            {"DEC [HL]",   3, &c::DEC,  R8(REG_H),   empt},            {"LD [HL], n8", 3, &c::LD,   n8,              R8(REG_HL_DATA)}, {"SCF",        1, &c::SCF,  empt,      empt},
		{"JR C, n8",     2, &c::JR,   n8,            COND(COND_C)},    {"ADD HL, SP", 2, &c::ADD,  R16(REG_SP), R16(REG_HL)},     {"LD A, [HL-]", 2, &c::LD,  R16MEM(REG_HLD), R8(REG_A)},       {"DEC SP",     2, &c::DEC, R16(REG_SP), empt},            {"INC A",        1, &c::INC,  R8(REG_A),       empt},            {"DEC A",      1, &c::DEC,  R8(REG_L),   empt},            {"LD A, n8",    2, &c::LD,   n8,              R8(REG_A)},       {"CCF",        1, &c::CCF,  empt,      empt},

		{"LD B, B",      1, &c::LD,   R8(REG_B),     R8(REG_B)},       {"LD B, C",    1, &c::LD,   R8(REG_C),   R8(REG_B)},       {"LD B, D",     1, &c::LD,  R8(REG_D),       R8(REG_B)},       {"LD B, E",    1, &c::LD,  R8(REG_E),   R8(REG_B)},       {"LD B, H",      1, &c::LD,   R8(REG_H),       R8(REG_B)},       {"LD B, L",    1, &c::LD,   R8(REG_L),   R8(REG_B)},       {"LD B, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_B)},       {"LD B, A",    1, &c::LD,   R8(REG_A), R8(REG_B)},
		{"LD C, B",      1, &c::LD,   R8(REG_B),     R8(REG_C)},       {"LD C, C",    1, &c::LD,   R8(REG_C),   R8(REG_C)},       {"LD C, D",     1, &c::LD,  R8(REG_D),       R8(REG_C)},       {"LD C, E",    1, &c::LD,  R8(REG_E),   R8(REG_C)},       {"LD C, H",      1, &c::LD,   R8(REG_H),       R8(REG_C)},       {"LD C, L",    1, &c::LD,   R8(REG_L),   R8(REG_C)},       {"LD C, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_C)},       {"LD C, A",    1, &c::LD,   R8(REG_A), R8(REG_C)},
		{"LD D, B",      1, &c::LD,   R8(REG_B),     R8(REG_D)},       {"LD D, C",    1, &c::LD,   R8(REG_C),   R8(REG_D)},       {"LD D, D",     1, &c::LD,  R8(REG_D),       R8(REG_D)},       {"LD D, E",    1, &c::LD,  R8(REG_E),   R8(REG_D)},       {"LD D, H",      1, &c::LD,   R8(REG_H),       R8(REG_D)},       {"LD D, L",    1, &c::LD,   R8(REG_L),   R8(REG_D)},       {"LD D, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_D)},       {"LD D, A",    1, &c::LD,   R8(REG_A), R8(REG_D)},
		{"LD E, B",      1, &c::LD,   R8(REG_B),     R8(REG_E)},       {"LD E, C",    1, &c::LD,   R8(REG_C),   R8(REG_E)},       {"LD E, D",     1, &c::LD,  R8(REG_D),       R8(REG_E)},       {"LD E, E",    1, &c::LD,  R8(REG_E),   R8(REG_E)},       {"LD E, H",      1, &c::LD,   R8(REG_H),       R8(REG_E)},       {"LD E, L",    1, &c::LD,   R8(REG_L),   R8(REG_E)},       {"LD E, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_E)},       {"LD E, A",    1, &c::LD,   R8(REG_A), R8(REG_E)},
		{"LD H, B",      1, &c::LD,   R8(REG_B),     R8(REG_H)},       {"LD H, C",    1, &c::LD,   R8(REG_C),   R8(REG_H)},       {"LD H, D",     1, &c::LD,  R8(REG_D),       R8(REG_H)},       {"LD H, E",    1, &c::LD,  R8(REG_E),   R8(REG_H)},       {"LD H, H",      1, &c::LD,   R8(REG_H),       R8(REG_H)},       {"LD H, L",    1, &c::LD,   R8(REG_L),   R8(REG_H)},       {"LD H, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_H)},       {"LD H, A",    1, &c::LD,   R8(REG_A), R8(REG_H)},
		{"LD L, B",      1, &c::LD,   R8(REG_B),     R8(REG_L)},       {"LD L, C",    1, &c::LD,   R8(REG_C),   R8(REG_L)},       {"LD L, D",     1, &c::LD,  R8(REG_D),       R8(REG_L)},       {"LD L, E",    1, &c::LD,  R8(REG_E),   R8(REG_L)},       {"LD L, H",      1, &c::LD,   R8(REG_H),       R8(REG_L)},       {"LD L, L",    1, &c::LD,   R8(REG_L),   R8(REG_L)},       {"LD L, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_L)},       {"LD L, A",    1, &c::LD,   R8(REG_A), R8(REG_L)},
		{"LD [HL], B",   2, &c::LD,   R8(REG_B),     R8(REG_HL_DATA)}, {"LD [HL], C", 2, &c::LD,   R8(REG_C),   R8(REG_HL_DATA)}, {"LD [HL], D",  2, &c::LD,  R8(REG_D),       R8(REG_HL_DATA)}, {"LD [HL], E", 2, &c::LD,  R8(REG_E),   R8(REG_HL_DATA)}, {"LD [HL], H",   2, &c::LD,   R8(REG_H),       R8(REG_HL_DATA)}, {"LD [HL], L", 2, &c::LD,   R8(REG_L),   R8(REG_HL_DATA)}, {"HALT",        1, &c::HALT, empt,            empt},            {"LD [HL], A", 1, &c::LD,   R8(REG_A), R8(REG_HL_DATA)},
		{"LD A, B",      1, &c::LD,   R8(REG_B),     R8(REG_A)},       {"LD A, C",    1, &c::LD,   R8(REG_C),   R8(REG_A)},       {"LD A, D",     1, &c::LD,  R8(REG_D),       R8(REG_A)},       {"LD A, E",    1, &c::LD,  R8(REG_E),   R8(REG_A)},       {"LD A, H",      1, &c::LD,   R8(REG_H),       R8(REG_A)},       {"LD A, L",    1, &c::LD,   R8(REG_L),   R8(REG_A)},       {"LD A, [HL]",  2, &c::LD,   R8(REG_HL_DATA), R8(REG_A)},       {"LD A, A",    1, &c::LD,   R8(REG_A), R8(REG_A)},

		{"ADD A, B",     1, &c::ADD,  R8(REG_B),     R8(REG_A)},       {"ADD A, C",   1, &c::ADD,  R8(REG_C),   R8(REG_A)},       {"ADD A, D",    1, &c::ADD, R8(REG_D),       R8(REG_A)},       {"ADD A, E",   1, &c::ADD, R8(REG_E),   R8(REG_A)},       {"ADD A, H",     1, &c::ADD,  R8(REG_H),       R8(REG_A)},       {"ADD A, L",   1, &c::ADD,  R8(REG_L),   R8(REG_A)},       {"ADD A, [HL]", 2, &c::ADD,  R8(REG_HL_DATA), R8(REG_A)},       {"ADD A, A",   1, &c::ADD,  R8(REG_A), R8(REG_A)},
		{"ADC A, B",     1, &c::ADC,  R8(REG_B),     R8(REG_A)},       {"ADC A, C",   1, &c::ADC,  R8(REG_C),   R8(REG_A)},       {"ADC A, D",    1, &c::ADC, R8(REG_D),       R8(REG_A)},       {"ADC A, E",   1, &c::ADC, R8(REG_E),   R8(REG_A)},       {"ADC A, H",     1, &c::ADC,  R8(REG_H),       R8(REG_A)},       {"ADC A, L",   1, &c::ADC,  R8(REG_L),   R8(REG_A)},       {"ADC A, [HL]", 2, &c::ADC,  R8(REG_HL_DATA), R8(REG_A)},       {"ADC A, A",   1, &c::ADC,  R8(REG_A), R8(REG_A)},
		{"SUB A, B",     1, &c::SUB,  R8(REG_B),     R8(REG_A)},       {"SUB A, C",   1, &c::SUB,  R8(REG_C),   R8(REG_A)},       {"SUB A, D",    1, &c::SUB, R8(REG_D),       R8(REG_A)},       {"SUB A, E",   1, &c::SUB, R8(REG_E),   R8(REG_A)},       {"SUB A, H",     1, &c::SUB,  R8(REG_H),       R8(REG_A)},       {"SUB A, L",   1, &c::SUB,  R8(REG_L),   R8(REG_A)},       {"SUB A, [HL]", 2, &c::SUB,  R8(REG_HL_DATA), R8(REG_A)},       {"SUB A, A",   1, &c::SUB,  R8(REG_A), R8(REG_A)},
		{"SBC A, B",     1, &c::SBC,  R8(REG_B),     R8(REG_A)},       {"SBC A, C",   1, &c::SBC,  R8(REG_C),   R8(REG_A)},       {"SBC A, D",    1, &c::SBC, R8(REG_D),       R8(REG_A)},       {"SBC A, E",   1, &c::SBC, R8(REG_E),   R8(REG_A)},       {"SBC A, H",     1, &c::SBC,  R8(REG_H),       R8(REG_A)},       {"SBC A, L",   1, &c::SBC,  R8(REG_L),   R8(REG_A)},       {"SBC A, [HL]", 2, &c::SBC,  R8(REG_HL_DATA), R8(REG_A)},       {"SBC A, A",   1, &c::SBC,  R8(REG_A), R8(REG_A)},
		{"AND A, B",     1, &c::AND,  R8(REG_B),     R8(REG_A)},       {"AND A, C",   1, &c::AND,  R8(REG_C),   R8(REG_A)},       {"AND A, D",    1, &c::AND, R8(REG_D),       R8(REG_A)},       {"AND A, E",   1, &c::AND, R8(REG_E),   R8(REG_A)},       {"AND A, H",     1, &c::AND,  R8(REG_H),       R8(REG_A)},       {"AND A, L",   1, &c::AND,  R8(REG_L),   R8(REG_A)},       {"AND A, [HL]", 2, &c::AND,  R8(REG_HL_DATA), R8(REG_A)},       {"AND A, A",   1, &c::AND,  R8(REG_A), R8(REG_A)},
		{"XOR A, B",     1, &c::XOR,  R8(REG_B),     R8(REG_A)},       {"XOR A, C",   1, &c::XOR,  R8(REG_C),   R8(REG_A)},       {"XOR A, D",    1, &c::XOR, R8(REG_D),       R8(REG_A)},       {"XOR A, E",   1, &c::XOR, R8(REG_E),   R8(REG_A)},       {"XOR A, H",     1, &c::XOR,  R8(REG_H),       R8(REG_A)},       {"XOR A, L",   1, &c::XOR,  R8(REG_L),   R8(REG_A)},       {"XOR A, [HL]", 2, &c::XOR,  R8(REG_HL_DATA), R8(REG_A)},       {"XOR A, A",   1, &c::XOR,  R8(REG_A), R8(REG_A)},
		{"OR A, B",      1, &c::OR,   R8(REG_B),     R8(REG_A)},       {"OR A, C",    1, &c::OR,   R8(REG_C),   R8(REG_A)},       {"OR A, D",     1, &c::OR,  R8(REG_D),       R8(REG_A)},       {"OR A, E",    1, &c::OR,  R8(REG_E),   R8(REG_A)},       {"OR A, H",      1, &c::OR,   R8(REG_H),       R8(REG_A)},       {"OR A, L",    1, &c::OR,   R8(REG_L),   R8(REG_A)},       {"OR A, [HL]",  2, &c::OR,   R8(REG_HL_DATA), R8(REG_A)},       {"OR A, A",    1, &c::OR,   R8(REG_A), R8(REG_A)},
		{"CP A, B",      1, &c::CP,   R8(REG_B),     R8(REG_A)},       {"CP A, C",    1, &c::CP,   R8(REG_C),   R8(REG_A)},       {"CP A, D",     1, &c::CP,  R8(REG_D),       R8(REG_A)},       {"CP A, E",    1, &c::CP,  R8(REG_E),   R8(REG_A)},       {"CP A, H",      1, &c::CP,   R8(REG_H),       R8(REG_A)},       {"CP A, L",    1, &c::CP,   R8(REG_L),   R8(REG_A)},       {"CP A, [HL]",  2, &c::CP,   R8(REG_HL_DATA), R8(REG_A)},       {"CP A, A",    1, &c::CP,   R8(REG_A), R8(REG_A)},

		{"RET NZ",       2, &c::RET,  COND(COND_NZ), empt},            {"POP BC",     3, &c::POP,  R16(REG_BC), empt},            {"JP NZ, a16",  3, &c::JP,  a16,             COND(COND_NZ)},   {"JP a16",     4, &c::JP,  a16,         empt},            {"CALL NZ, a16", 3, &c::CALL, a16,             COND(COND_NZ)},   {"PUSH BC",    4, &c::PUSH, R16(REG_BC), empt},            {"ADD A, n8",   2, &c::ADD,  n8,              R8(REG_A)},       {"RST $00",    4, &c::RST,  VEC(0x00), empt},
		{"RET Z",        2, &c::RET,  COND(COND_Z),  empt},            {"RET",        4, &c::RET,  empt,        empt},            {"JP Z, a16",   3, &c::JP,  a16,             COND(COND_Z)},    {"PREFIX",     1, &c::CB,  empt,        empt},            {"CALL Z, a16",  3, &c::CALL, a16,             COND(COND_Z)},    {"CALL a16",   6, &c::CALL, a16,         empt},            {"ADC A, n8",   2, &c::ADC,  n8,              R8(REG_A)},       {"RST $08",    4, &c::RST,  VEC(0x08), empt},
		{"RET NC",       2, &c::RET,  COND(COND_NC), empt},            {"POP DE",     3, &c::POP,  R16(REG_DE), empt},            {"JP NC, a16",  3, &c::JP,  a16,             COND(COND_NC)},   {"INVALID",    1, &c::INV, empt,        empt},            {"CALL NC, a16", 3, &c::CALL, a16,             COND(COND_NC)},   {"PUSH DE",    4, &c::PUSH, R16(REG_DE), empt},            {"SUB A, n8",   2, &c::SUB,  n8,              R8(REG_A)},       {"RST $10",    4, &c::RST,  VEC(0x10), empt},
		{"RET C",        2, &c::RET,  COND(COND_C),  empt},            {"RETI",       4, &c::RETI, empt,        empt},            {"JP C, a16",   3, &c::JP,  a16,             COND(COND_C)},    {"INVALID",    1, &c::INV, empt,        empt},            {"CALL C, a16",  3, &c::CALL, a16,             COND(COND_C)},    {"INVALID",    1, &c::INV,  empt,        empt},            {"SBC A, n8",   2, &c::SBC,  n8,              R8(REG_A)},       {"RST $18",    4, &c::RST,  VEC(0x18), empt},
		{"LDH a8, A",    3, &c::LDH,  a8,            R8(REG_A)},       {"POP HL",     3, &c::POP,  R16(REG_HL), empt},            {"LDH [C], A",  2, &c::LDH, R8(REG_C),       R8(REG_A)},       {"INVALID",    1, &c::INV, empt,        empt},            {"INVALID",      1, &c::INV,  empt,            empt},            {"PUSH HL",    4, &c::PUSH, R16(REG_HL), empt},            {"AND A, n8",   2, &c::AND,  n8,              R8(REG_A)},       {"RST $20",    4, &c::RST,  VEC(0x20), empt},
		{"ADD SP, e8",   4, &c::ADD,  n8,            R16(REG_SP)},     {"JP HL",      1, &c::JP,   R16(REG_HL), empt},            {"LD a16, A",   4, &c::LD,  a16,             R8(REG_A)},       {"INVALID",    1, &c::INV, empt,        empt},            {"INVALID",      1, &c::INV,  empt,            empt},            {"INVALID",    1, &c::INV,  empt,        empt},            {"XOR A, n8",   2, &c::XOR,  n8,              R8(REG_A)},       {"RST $28",    4, &c::RST,  VEC(0x28), empt},
		{"LDH a8",       3, &c::LDH,  R8(REG_A),     a8},              {"POP AF",     3, &c::POP,  R16(REG_AF), empt},            {"LDH A, [C]",  2, &c::LDH, R8(REG_A),       R8(REG_C)},       {"DI",         1, &c::DI,  empt,        empt},            {"INVALID",      1, &c::INV,  empt,            empt},            {"PUSH AF",    4, &c::PUSH, R16(REG_AF), empt},            {"OR A, n8",    2, &c::OR,   n8,              R8(REG_A)},       {"RST $30",    4, &c::RST,  VEC(0x30), empt},
		{"LD HL, SP+n8", 3, &c::LD,   empt, /*fix*/  empt},            {"LD SP, HL",  2, &c::LD,   R16(REG_HL), R16(REG_SP)},     {"LD A, a16",   4, &c::LD,  a16,             R8(REG_A)},       {"EI",         1, &c::EI,  empt,        empt},            {"INVALID",      1, &c::INV,  empt,            empt},            {"INVALID",    1, &c::INV,  empt,        empt},            {"CP A, n8",    2, &c::CP,   n8,              R8(REG_A)},       {"RST $38",    4, &c::RST,  VEC(0x38), empt},
	};

	cb_lookup =
	{

		{"RLC B",    2, &c::RLC,  R8(REG_B), empt},      {"RLC C",    2, &c::RLC,  R8(REG_C), empt},      {"RLC D",    2, &c::RLC,  R8(REG_D), empt},      {"RLC E",    2, &c::RLC,  R8(REG_E), empt},      {"RLC H",    2, &c::RLC,  R8(REG_H), empt},      {"RLC L",    2, &c::RLC,  R8(REG_L), empt},      {"RLC [HL]",    4, &c::RLC,  R8(REG_HL_DATA), empt},            {"RLC A",    2, &c::RLC,  R8(REG_A), empt},
		{"RRC B",    2, &c::RRC,  R8(REG_B), empt},      {"RRC C",    2, &c::RRC,  R8(REG_C), empt},      {"RRC D",    2, &c::RRC,  R8(REG_D), empt},      {"RRC E",    2, &c::RRC,  R8(REG_E), empt},      {"RRC H",    2, &c::RRC,  R8(REG_H), empt},      {"RRC L",    2, &c::RRC,  R8(REG_L), empt},      {"RRC [HL]",    4, &c::RRC,  R8(REG_HL_DATA), empt},            {"RRC A",    2, &c::RRC,  R8(REG_A), empt},
		{"RL B",     2, &c::RL,   R8(REG_B), empt},      {"RL C",     2, &c::RL,   R8(REG_C), empt},      {"RL D",     2, &c::RL,   R8(REG_D), empt},      {"RL E",     2, &c::RL,   R8(REG_E), empt},      {"RL H",     2, &c::RL,   R8(REG_H), empt},      {"RL L",     2, &c::RL,   R8(REG_L), empt},      {"RL [HL]",     4, &c::RL,   R8(REG_HL_DATA), empt},            {"RL A",     2, &c::RL,   R8(REG_A), empt},
		{"RR B",     2, &c::RR,   R8(REG_B), empt},      {"RR C",     2, &c::RR,   R8(REG_C), empt},      {"RR D",     2, &c::RR,   R8(REG_D), empt},      {"RR E",     2, &c::RR,   R8(REG_E), empt},      {"RR H",     2, &c::RR,   R8(REG_H), empt},      {"RR L",     2, &c::RR,   R8(REG_L), empt},      {"RR [HL]",     4, &c::RR,   R8(REG_HL_DATA), empt},            {"RR A",     2, &c::RR,   R8(REG_A), empt},
		{"SLA B",    2, &c::SLA,  R8(REG_B), empt},      {"SLA C",    2, &c::SLA,  R8(REG_C), empt},      {"SLA D",    2, &c::SLA,  R8(REG_D), empt},      {"SLA E",    2, &c::SLA,  R8(REG_E), empt},      {"SLA H",    2, &c::SLA,  R8(REG_H), empt},      {"SLA L",    2, &c::SLA,  R8(REG_L), empt},      {"SLA [HL]",    4, &c::SLA,  R8(REG_HL_DATA), empt},            {"SLA A",    2, &c::SLA,  R8(REG_A), empt},
		{"SRA B",    2, &c::SRA,  R8(REG_B), empt},      {"SRA C",    2, &c::SRA,  R8(REG_C), empt},      {"SRA D",    2, &c::SRA,  R8(REG_D), empt},      {"SRA E",    2, &c::SRA,  R8(REG_E), empt},      {"SRA H",    2, &c::SRA,  R8(REG_H), empt},      {"SRA L",    2, &c::SRA,  R8(REG_L), empt},      {"SRA [HL]",    4, &c::SRA,  R8(REG_HL_DATA), empt},            {"SRA A",    2, &c::SRA,  R8(REG_A), empt},
		{"SWAP B",   2, &c::SWAP, R8(REG_B), empt},      {"SWAP C",   2, &c::SWAP, R8(REG_C), empt},      {"SWAP D",   2, &c::SWAP, R8(REG_D), empt},      {"SWAP E",   2, &c::SWAP, R8(REG_E), empt},      {"SWAP H",   2, &c::SWAP, R8(REG_H), empt},      {"SWAP L",   2, &c::SWAP, R8(REG_L), empt},      {"SWAP [HL]",   4, &c::SWAP, R8(REG_HL_DATA), empt},            {"SWAP A",   2, &c::SWAP, R8(REG_A), empt},
		{"SRL B",    2, &c::SRL,  R8(REG_B), empt},      {"SRL C",    2, &c::SRL,  R8(REG_C), empt},      {"SRL D",    2, &c::SRL,  R8(REG_D), empt},      {"SRL E",    2, &c::SRL,  R8(REG_E), empt},      {"SRL H",    2, &c::SRL,  R8(REG_H), empt},      {"SRL L",    2, &c::SRL,  R8(REG_L), empt},      {"SRL [HL]",    4, &c::SRL,  R8(REG_HL_DATA), empt},            {"SRL A",    2, &c::SRL,  R8(REG_A), empt},

		{"BIT 0, B", 2, &c::BIT,  B3(0),     R8(REG_B)}, {"BIT 0, C", 2, &c::BIT,  B3(0),     R8(REG_C)}, {"BIT 0, D", 2, &c::BIT,  B3(0),     R8(REG_D)}, {"BIT 0, E", 2, &c::BIT,  B3(0),     R8(REG_E)}, {"BIT 0, H", 2, &c::BIT,  B3(0),     R8(REG_H)}, {"BIT 0, L", 2, &c::BIT,  B3(0),     R8(REG_L)}, {"BIT 0, [HL]", 3, &c::BIT,  B3(0),           R8(REG_HL_DATA)}, {"BIT 0, A", 2, &c::BIT,  B3(0),     R8(REG_A)},
		{"BIT 1, B", 2, &c::BIT,  B3(1),     R8(REG_B)}, {"BIT 1, C", 2, &c::BIT,  B3(1),     R8(REG_C)}, {"BIT 1, D", 2, &c::BIT,  B3(1),     R8(REG_D)}, {"BIT 1, E", 2, &c::BIT,  B3(1),     R8(REG_E)}, {"BIT 1, H", 2, &c::BIT,  B3(1),     R8(REG_H)}, {"BIT 1, L", 2, &c::BIT,  B3(1),     R8(REG_L)}, {"BIT 1, [HL]", 3, &c::BIT,  B3(1),           R8(REG_HL_DATA)}, {"BIT 1, A", 2, &c::BIT,  B3(1),     R8(REG_A)},
		{"BIT 2, B", 2, &c::BIT,  B3(2),     R8(REG_B)}, {"BIT 2, C", 2, &c::BIT,  B3(2),     R8(REG_C)}, {"BIT 2, D", 2, &c::BIT,  B3(2),     R8(REG_D)}, {"BIT 2, E", 2, &c::BIT,  B3(2),     R8(REG_E)}, {"BIT 2, H", 2, &c::BIT,  B3(2),     R8(REG_H)}, {"BIT 2, L", 2, &c::BIT,  B3(2),     R8(REG_L)}, {"BIT 2, [HL]", 3, &c::BIT,  B3(2),           R8(REG_HL_DATA)}, {"BIT 2, A", 2, &c::BIT,  B3(2),     R8(REG_A)},
		{"BIT 3, B", 2, &c::BIT,  B3(3),     R8(REG_B)}, {"BIT 3, C", 2, &c::BIT,  B3(3),     R8(REG_C)}, {"BIT 3, D", 2, &c::BIT,  B3(3),     R8(REG_D)}, {"BIT 3, E", 2, &c::BIT,  B3(3),     R8(REG_E)}, {"BIT 3, H", 2, &c::BIT,  B3(3),     R8(REG_H)}, {"BIT 3, L", 2, &c::BIT,  B3(3),     R8(REG_L)}, {"BIT 3, [HL]", 3, &c::BIT,  B3(3),           R8(REG_HL_DATA)}, {"BIT 3, A", 2, &c::BIT,  B3(3),     R8(REG_A)},
		{"BIT 4, B", 2, &c::BIT,  B3(4),     R8(REG_B)}, {"BIT 4, C", 2, &c::BIT,  B3(4),     R8(REG_C)}, {"BIT 4, D", 2, &c::BIT,  B3(4),     R8(REG_D)}, {"BIT 4, E", 2, &c::BIT,  B3(4),     R8(REG_E)}, {"BIT 4, H", 2, &c::BIT,  B3(4),     R8(REG_H)}, {"BIT 4, L", 2, &c::BIT,  B3(4),     R8(REG_L)}, {"BIT 4, [HL]", 3, &c::BIT,  B3(4),           R8(REG_HL_DATA)}, {"BIT 4, A", 2, &c::BIT,  B3(4),     R8(REG_A)},
		{"BIT 5, B", 2, &c::BIT,  B3(5),     R8(REG_B)}, {"BIT 5, C", 2, &c::BIT,  B3(5),     R8(REG_C)}, {"BIT 5, D", 2, &c::BIT,  B3(5),     R8(REG_D)}, {"BIT 5, E", 2, &c::BIT,  B3(5),     R8(REG_E)}, {"BIT 5, H", 2, &c::BIT,  B3(5),     R8(REG_H)}, {"BIT 5, L", 2, &c::BIT,  B3(5),     R8(REG_L)}, {"BIT 5, [HL]", 3, &c::BIT,  B3(5),           R8(REG_HL_DATA)}, {"BIT 5, A", 2, &c::BIT,  B3(5),     R8(REG_A)},
		{"BIT 6, B", 2, &c::BIT,  B3(6),     R8(REG_B)}, {"BIT 6, C", 2, &c::BIT,  B3(6),     R8(REG_C)}, {"BIT 6, D", 2, &c::BIT,  B3(6),     R8(REG_D)}, {"BIT 6, E", 2, &c::BIT,  B3(6),     R8(REG_E)}, {"BIT 6, H", 2, &c::BIT,  B3(6),     R8(REG_H)}, {"BIT 6, L", 2, &c::BIT,  B3(6),     R8(REG_L)}, {"BIT 6, [HL]", 3, &c::BIT,  B3(6),           R8(REG_HL_DATA)}, {"BIT 6, A", 2, &c::BIT,  B3(6),     R8(REG_A)},
		{"BIT 7, B", 2, &c::BIT,  B3(7),     R8(REG_B)}, {"BIT 7, C", 2, &c::BIT,  B3(7),     R8(REG_C)}, {"BIT 7, D", 2, &c::BIT,  B3(7),     R8(REG_D)}, {"BIT 7, E", 2, &c::BIT,  B3(7),     R8(REG_E)}, {"BIT 7, H", 2, &c::BIT,  B3(7),     R8(REG_H)}, {"BIT 7, L", 2, &c::BIT,  B3(7),     R8(REG_L)}, {"BIT 7, [HL]", 3, &c::BIT,  B3(7),           R8(REG_HL_DATA)}, {"BIT 7, A", 2, &c::BIT,  B3(7),     R8(REG_A)},
																																																																					  
		{"RES 0, B", 2, &c::RES,  B3(0),     R8(REG_B)}, {"RES 0, C", 2, &c::RES,  B3(0),     R8(REG_C)}, {"RES 0, D", 2, &c::RES,  B3(0),     R8(REG_D)}, {"RES 0, E", 2, &c::RES,  B3(0),     R8(REG_E)}, {"RES 0, H", 2, &c::RES,  B3(0),     R8(REG_H)}, {"RES 0, L", 2, &c::RES,  B3(0),     R8(REG_L)}, {"RES 0, [HL]", 4, &c::RES,  B3(0),           R8(REG_HL_DATA)}, {"RES 0, A", 2, &c::RES,  B3(0),     R8(REG_A)},
		{"RES 1, B", 2, &c::RES,  B3(1),     R8(REG_B)}, {"RES 1, C", 2, &c::RES,  B3(1),     R8(REG_C)}, {"RES 1, D", 2, &c::RES,  B3(1),     R8(REG_D)}, {"RES 1, E", 2, &c::RES,  B3(1),     R8(REG_E)}, {"RES 1, H", 2, &c::RES,  B3(1),     R8(REG_H)}, {"RES 1, L", 2, &c::RES,  B3(1),     R8(REG_L)}, {"RES 1, [HL]", 4, &c::RES,  B3(1),           R8(REG_HL_DATA)}, {"RES 1, A", 2, &c::RES,  B3(1),     R8(REG_A)},
		{"RES 2, B", 2, &c::RES,  B3(2),     R8(REG_B)}, {"RES 2, C", 2, &c::RES,  B3(2),     R8(REG_C)}, {"RES 2, D", 2, &c::RES,  B3(2),     R8(REG_D)}, {"RES 2, E", 2, &c::RES,  B3(2),     R8(REG_E)}, {"RES 2, H", 2, &c::RES,  B3(2),     R8(REG_H)}, {"RES 2, L", 2, &c::RES,  B3(2),     R8(REG_L)}, {"RES 2, [HL]", 4, &c::RES,  B3(2),           R8(REG_HL_DATA)}, {"RES 2, A", 2, &c::RES,  B3(2),     R8(REG_A)},
		{"RES 3, B", 2, &c::RES,  B3(3),     R8(REG_B)}, {"RES 3, C", 2, &c::RES,  B3(3),     R8(REG_C)}, {"RES 3, D", 2, &c::RES,  B3(3),     R8(REG_D)}, {"RES 3, E", 2, &c::RES,  B3(3),     R8(REG_E)}, {"RES 3, H", 2, &c::RES,  B3(3),     R8(REG_H)}, {"RES 3, L", 2, &c::RES,  B3(3),     R8(REG_L)}, {"RES 3, [HL]", 4, &c::RES,  B3(3),           R8(REG_HL_DATA)}, {"RES 3, A", 2, &c::RES,  B3(3),     R8(REG_A)},
		{"RES 4, B", 2, &c::RES,  B3(4),     R8(REG_B)}, {"RES 4, C", 2, &c::RES,  B3(4),     R8(REG_C)}, {"RES 4, D", 2, &c::RES,  B3(4),     R8(REG_D)}, {"RES 4, E", 2, &c::RES,  B3(4),     R8(REG_E)}, {"RES 4, H", 2, &c::RES,  B3(4),     R8(REG_H)}, {"RES 4, L", 2, &c::RES,  B3(4),     R8(REG_L)}, {"RES 4, [HL]", 4, &c::RES,  B3(4),           R8(REG_HL_DATA)}, {"RES 4, A", 2, &c::RES,  B3(4),     R8(REG_A)},
		{"RES 5, B", 2, &c::RES,  B3(5),     R8(REG_B)}, {"RES 5, C", 2, &c::RES,  B3(5),     R8(REG_C)}, {"RES 5, D", 2, &c::RES,  B3(5),     R8(REG_D)}, {"RES 5, E", 2, &c::RES,  B3(5),     R8(REG_E)}, {"RES 5, H", 2, &c::RES,  B3(5),     R8(REG_H)}, {"RES 5, L", 2, &c::RES,  B3(5),     R8(REG_L)}, {"RES 5, [HL]", 4, &c::RES,  B3(5),           R8(REG_HL_DATA)}, {"RES 5, A", 2, &c::RES,  B3(5),     R8(REG_A)},
		{"RES 6, B", 2, &c::RES,  B3(6),     R8(REG_B)}, {"RES 6, C", 2, &c::RES,  B3(6),     R8(REG_C)}, {"RES 6, D", 2, &c::RES,  B3(6),     R8(REG_D)}, {"RES 6, E", 2, &c::RES,  B3(6),     R8(REG_E)}, {"RES 6, H", 2, &c::RES,  B3(6),     R8(REG_H)}, {"RES 6, L", 2, &c::RES,  B3(6),     R8(REG_L)}, {"RES 6, [HL]", 4, &c::RES,  B3(6),           R8(REG_HL_DATA)}, {"RES 6, A", 2, &c::RES,  B3(6),     R8(REG_A)},
		{"RES 7, B", 2, &c::RES,  B3(7),     R8(REG_B)}, {"RES 7, C", 2, &c::RES,  B3(7),     R8(REG_C)}, {"RES 7, D", 2, &c::RES,  B3(7),     R8(REG_D)}, {"RES 7, E", 2, &c::RES,  B3(7),     R8(REG_E)}, {"RES 7, H", 2, &c::RES,  B3(7),     R8(REG_H)}, {"RES 7, L", 2, &c::RES,  B3(7),     R8(REG_L)}, {"RES 7, [HL]", 4, &c::RES,  B3(7),           R8(REG_HL_DATA)}, {"RES 7, A", 2, &c::RES,  B3(7),     R8(REG_A)},
																																																																					  
		{"SET 0, B", 2, &c::SET,  B3(0),     R8(REG_B)}, {"SET 0, C", 2, &c::SET,  B3(0),     R8(REG_C)}, {"SET 0, D", 2, &c::SET,  B3(0),     R8(REG_D)}, {"SET 0, E", 2, &c::SET,  B3(0),     R8(REG_E)}, {"SET 0, H", 2, &c::SET,  B3(0),     R8(REG_H)}, {"SET 0, L", 2, &c::SET,  B3(0),     R8(REG_L)}, {"SET 0, [HL]", 4, &c::SET,  B3(0),           R8(REG_HL_DATA)}, {"SET 0, A", 2, &c::SET,  B3(0),     R8(REG_A)},
		{"SET 1, B", 2, &c::SET,  B3(1),     R8(REG_B)}, {"SET 1, C", 2, &c::SET,  B3(1),     R8(REG_C)}, {"SET 1, D", 2, &c::SET,  B3(1),     R8(REG_D)}, {"SET 1, E", 2, &c::SET,  B3(1),     R8(REG_E)}, {"SET 1, H", 2, &c::SET,  B3(1),     R8(REG_H)}, {"SET 1, L", 2, &c::SET,  B3(1),     R8(REG_L)}, {"SET 1, [HL]", 4, &c::SET,  B3(1),           R8(REG_HL_DATA)}, {"SET 1, A", 2, &c::SET,  B3(1),     R8(REG_A)},
		{"SET 2, B", 2, &c::SET,  B3(2),     R8(REG_B)}, {"SET 2, C", 2, &c::SET,  B3(2),     R8(REG_C)}, {"SET 2, D", 2, &c::SET,  B3(2),     R8(REG_D)}, {"SET 2, E", 2, &c::SET,  B3(2),     R8(REG_E)}, {"SET 2, H", 2, &c::SET,  B3(2),     R8(REG_H)}, {"SET 2, L", 2, &c::SET,  B3(2),     R8(REG_L)}, {"SET 2, [HL]", 4, &c::SET,  B3(2),           R8(REG_HL_DATA)}, {"SET 2, A", 2, &c::SET,  B3(2),     R8(REG_A)},
		{"SET 3, B", 2, &c::SET,  B3(3),     R8(REG_B)}, {"SET 3, C", 2, &c::SET,  B3(3),     R8(REG_C)}, {"SET 3, D", 2, &c::SET,  B3(3),     R8(REG_D)}, {"SET 3, E", 2, &c::SET,  B3(3),     R8(REG_E)}, {"SET 3, H", 2, &c::SET,  B3(3),     R8(REG_H)}, {"SET 3, L", 2, &c::SET,  B3(3),     R8(REG_L)}, {"SET 3, [HL]", 4, &c::SET,  B3(3),           R8(REG_HL_DATA)}, {"SET 3, A", 2, &c::SET,  B3(3),     R8(REG_A)},
		{"SET 4, B", 2, &c::SET,  B3(4),     R8(REG_B)}, {"SET 4, C", 2, &c::SET,  B3(4),     R8(REG_C)}, {"SET 4, D", 2, &c::SET,  B3(4),     R8(REG_D)}, {"SET 4, E", 2, &c::SET,  B3(4),     R8(REG_E)}, {"SET 4, H", 2, &c::SET,  B3(4),     R8(REG_H)}, {"SET 4, L", 2, &c::SET,  B3(4),     R8(REG_L)}, {"SET 4, [HL]", 4, &c::SET,  B3(4),           R8(REG_HL_DATA)}, {"SET 4, A", 2, &c::SET,  B3(4),     R8(REG_A)},
		{"SET 5, B", 2, &c::SET,  B3(5),     R8(REG_B)}, {"SET 5, C", 2, &c::SET,  B3(5),     R8(REG_C)}, {"SET 5, D", 2, &c::SET,  B3(5),     R8(REG_D)}, {"SET 5, E", 2, &c::SET,  B3(5),     R8(REG_E)}, {"SET 5, H", 2, &c::SET,  B3(5),     R8(REG_H)}, {"SET 5, L", 2, &c::SET,  B3(5),     R8(REG_L)}, {"SET 5, [HL]", 4, &c::SET,  B3(5),           R8(REG_HL_DATA)}, {"SET 5, A", 2, &c::SET,  B3(5),     R8(REG_A)},
		{"SET 6, B", 2, &c::SET,  B3(6),     R8(REG_B)}, {"SET 6, C", 2, &c::SET,  B3(6),     R8(REG_C)}, {"SET 6, D", 2, &c::SET,  B3(6),     R8(REG_D)}, {"SET 6, E", 2, &c::SET,  B3(6),     R8(REG_E)}, {"SET 6, H", 2, &c::SET,  B3(6),     R8(REG_H)}, {"SET 6, L", 2, &c::SET,  B3(6),     R8(REG_L)}, {"SET 6, [HL]", 4, &c::SET,  B3(6),           R8(REG_HL_DATA)}, {"SET 6, A", 2, &c::SET,  B3(6),     R8(REG_A)},
		{"SET 7, B", 2, &c::SET,  B3(7),     R8(REG_B)}, {"SET 7, C", 2, &c::SET,  B3(7),     R8(REG_C)}, {"SET 7, D", 2, &c::SET,  B3(7),     R8(REG_D)}, {"SET 7, E", 2, &c::SET,  B3(7),     R8(REG_E)}, {"SET 7, H", 2, &c::SET,  B3(7),     R8(REG_H)}, {"SET 7, L", 2, &c::SET,  B3(7),     R8(REG_L)}, {"SET 7, [HL]", 4, &c::SET,  B3(7),           R8(REG_HL_DATA)}, {"SET 7, A", 2, &c::SET,  B3(7),     R8(REG_A)},
	};
}

CPU::~CPU() {

}

bool CPU::getFlag(uint8_t flag) {
	return af.low & flag;
}

void CPU::setFlag(uint8_t flag) {
	af.low |= flag;
}

void CPU::clearFlag(uint8_t flag) {
	af.low &= ~flag;
}

void CPU::putFlag(uint8_t flag, bool value) {
	value ? setFlag(flag) : clearFlag(flag);
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

uint8_t CPU::read_r8(uint8_t selector) {
	switch (selector) {
	case REG_B:
		return bc.high;
	case REG_C:
		return bc.low;
	case REG_D:
		return de.high;
	case REG_E:
		return de.low;
	case REG_H:
		return hl.high;
	case REG_L:
		return hl.low;
	case REG_HL_DATA:
		return read(hl.reg);
	case REG_A:
		return af.high;
	}
	throw std::runtime_error("Invalid r8 selection");
}

void CPU::write_r8(uint8_t selector, uint8_t data) {
	switch (selector) {
	case REG_B:
		bc.high = data;
	case REG_C:
		bc.low = data;
	case REG_D:
		de.high = data;
	case REG_E:
		de.low = data;
	case REG_H:
		hl.high = data;
	case REG_L:
		hl.low = data;
	case REG_HL_DATA:
		write(hl.reg, data);
	case REG_A:
		af.high = data;
	}
	throw std::runtime_error("Invalid r8 selection");
}

uint16_t CPU::read_r16(uint8_t selector) {
	switch (selector) {
	case REG_BC:
		return bc.reg;
	case REG_DE:
		return de.reg;
	case REG_HL:
		return hl.reg;
	case REG_SP:
		return sp;
	}
	throw std::runtime_error("Invalid r16 selection");
}

void CPU::write_r16(uint8_t selector, uint16_t data) {
	switch (selector) {
	case REG_BC:
		bc.reg = data;
	case REG_DE:
		de.reg = data;
	case REG_HL:
		hl.reg = data;
	case REG_SP:
		sp = data;
	}
	throw std::runtime_error("Invalid r16 selection");
}

uint16_t CPU::read_r16stk(uint8_t selector) {
	switch (selector) {
	case REG_BC:
		return bc.reg;
	case REG_DE:
		return de.reg;
	case REG_HL:
		return hl.reg;
	case REG_AF:
		return af.reg;
	}
	throw std::runtime_error("Invalid r16stk selection");
}

void CPU::write_r16stk(uint8_t selector, uint16_t data) {
	switch (selector) {
	case REG_BC:
		bc.reg = data;
	case REG_DE:
		de.reg = data;
	case REG_HL:
		hl.reg = data;
	case REG_AF:
		af.reg = data;
	}
	throw std::runtime_error("Invalid r16stk selection");
}

uint8_t CPU::read_r16mem(uint8_t selector) {
	switch (selector) {
	case REG_BC:
		return read(bc.reg);
	case REG_DE:
		return read(de.reg);
	case REG_HLI:
		return read(hl.reg++);
	case REG_HLD:
		return read(hl.reg--);
	}
	throw std::runtime_error("Invalid r16mem selection");
}

void CPU::write_r16mem(uint8_t selector, uint8_t data) {
	switch (selector) {
	case REG_BC:
		write(bc.reg, data);
	case REG_DE:
		write(de.reg, data);
	case REG_HLI:
		write(hl.reg++, data);
	case REG_HLD:
		write(hl.reg--, data);
	}
	throw std::runtime_error("Invalid r16mem selection");
}

bool CPU::check_cond(uint8_t selector) {
	switch (selector) {
	case COND_NZ:
		return (AF() & 0b10000000) == 0;
	case COND_Z:
		return (AF() & 0b10000000);
	case COND_NC:
		return (AF() & 0b00010000) == 0;
	case COND_C:
		return (AF() & 0b00010000);
	}
	throw std::runtime_error("Invalid cond selection");
}


void CPU::decode(uint8_t opcode) {
	
}

uint16_t CPU::readOperand(Operand op) {
	switch (op.type) {
	case OperandType::R8:     return read_r8(op.index);
	case OperandType::R16:    return read_r16(op.index);
	case OperandType::R16STK: return read_r16stk(op.index);
	case OperandType::R16MEM: return read_r16mem(op.index);
	case OperandType::COND:   return check_cond(op.index);
	case OperandType::VEC:    return op.index;
	case OperandType::B3:     return op.index;
	case OperandType::n8:     return fetchByte();
	case OperandType::n16:    return fetchWord();
	case OperandType::a8:     return read(0xFF00 | fetchByte());
	case OperandType::a16:    return read(fetchWord());
	}
	throw std::runtime_error("readOperand: Invalid operand type");
}

void CPU::writeOperand(Operand op, uint16_t value) {
	switch (op.type) {
	case OperandType::R8: write_r8(op.index, value & 0xFF); break;
	case OperandType::R16: write_r16(op.index, value); break;
	case OperandType::R16STK: write_r16stk(op.index, value); break;
	case OperandType::R16MEM: write_r16mem(op.index, value & 0xFF); break;
	//case OperandType::B3: (write flag op.index, value)
	//case OperandType::a8
	//case OperandType::a16
	// invalid: cond, vec, n8, n16
	}
	throw std::runtime_error("writeOperand: Invalid operand type");
}

bool checkOverflow(int bit, uint16_t num1, uint16_t num2, bool carry) {
	num1 >> bit;
	num2 >> bit;

	num1 << bit;
	num2 << bit;

	uint32_t low1 = (uint32_t)(num1 & (~num1));
	uint32_t low2 = (uint32_t)(num2 & (~num2));
	
	uint32_t sum = low1 + low2 + carry;
	return sum >> bit;
}

// Check all instructions for flag changes
// Implement flag functions

/*

Check off the following instructions to make sure all are working properly:

Load instructions
LD r8,r8
LD r8,n8
LD r16,n16
LD [HL],r8
LD [HL],n8
LD r8,[HL]
LD [r16],A
LD [n16],A

LDH [n16],A
LDH [C],A

LD A,[r16]
LD A,[n16]

LDH A,[n16]
LDH A,[C]

LD [HLI],A
LD [HLD],A
LD A,[HLI]
LD A,[HLD]

8-bit arithmetic instructions
ADC A,r8
ADC A,[HL]
ADC A,n8

ADD A,r8
ADD A,[HL]
ADD A,n8

CP A,r8
CP A,[HL]
CP A,n8

DEC r8
DEC [HL]

INC r8
INC [HL]

SBC A,r8
SBC A,[HL]
SBC A,n8

SUB A,r8
SUB A,[HL]
SUB A,n8

16-bit arithmetic instructions
ADD HL,r16
DEC r16
INC r16

Bitwise logic instructions
AND A,r8
AND A,[HL]
AND A,n8
CPL
OR A,r8
OR A,[HL]
OR A,n8
XOR A,r8
XOR A,[HL]
XOR A,n8

Bit flag instructions
BIT u3,r8
BIT u3,[HL]
RES u3,r8
RES u3,[HL]
SET u3,r8
SET u3,[HL]

Bit shift instructions
RL r8
RL [HL]
RLA
RLC r8
RLC [HL]
RLCA
RR r8
RR [HL]
RRA
RRC r8
RRC [HL]
RRCA
SLA r8
SLA [HL]
SRA r8
SRA [HL]
SRL r8
SRL [HL]
SWAP r8
SWAP [HL]

Jumps and subroutine instructions
CALL n16
CALL cc,n16
JP HL
JP n16
JP cc,n16
JR n16
JR cc,n16
RET cc
RET
RETI
RST vec

Carry flag instructions
CCF
SCF

Stack manipulation instructions
ADD HL,SP
ADD SP,e8
DEC SP
INC SP
LD SP,n16
LD [n16],SP
LD HL,SP+e8
LD SP,HL
POP AF
POP r16
PUSH AF
PUSH r16

Interrupt-related instructions
DI
EI
HALT

Miscellaneous instructions
DAA
NOP
STOP

*/

// Loads
void CPU::LD(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(dst, src_v);
}

void CPU::LDH(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(dst, src_v);
}

// Arithmetic
void CPU::ADC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = src_v + dst_v + getFlag(FLAG_C);
	writeOperand(dst, sum);
	
	putFlag(FLAG_Z, (sum == 0));
	clearFlag(FLAG_N);
	putFlag(FLAG_H, checkOverflow(3, src_v, dst_v, getFlag(FLAG_C)));
	putFlag(FLAG_C, checkOverflow(7, src_v, dst_v, getFlag(FLAG_C)));
}

void CPU::ADD(Operand src, Operand dst) {
	bool is16bit = false;
	bool isSP = false;
	if ((src.type == OperandType::R16) || (src.type == OperandType::R16STK)) {
		is16bit = true;
	}
	// TODO: CHECK FOR SP, e8 ADDITION

	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = src_v + dst_v;
	writeOperand(dst, sum);

	if (is16bit) {
		clearFlag(FLAG_N);
		putFlag(FLAG_H, checkOverflow(11, src_v, dst_v, false));
		putFlag(FLAG_C, checkOverflow(15, src_v, dst_v, false));
	}
	else {
		putFlag(FLAG_Z, !isSP && (sum == 0));
		clearFlag(FLAG_N);
		putFlag(FLAG_H, checkOverflow(3, src_v, dst_v, false));
		putFlag(FLAG_C, checkOverflow(7, src_v, dst_v, false));
	}
}

void CPU::SBC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = dst_v - src_v - getFlag(FLAG_C);
	writeOperand(dst, sum);

	putFlag(FLAG_Z, sum == 0);
	setFlag(FLAG_N);
	putFlag(FLAG_H, (src & 0xF) > ((dst + getFlag(FLAG_C)) & 0xF));
	putFlag(FLAG_C, src > (dst + getFlag(FLAG_C));
}

void CPU::SUB(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = dst_v - src_v;
	writeOperand(dst, sum);

	putFlag(FLAG_Z, sum == 0);
	setFlag(FLAG_N);
	putFlag(FLAG_H, (src & 0xF) > (dst & 0xF));
	putFlag(FLAG_C, src > dst);
}

void CPU::INC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(dst, src_v++);

	if (src.type == OperandType::R8) {
		putFlag(FLAG_Z, sum == 0);
		clearFlag(FLAG_N);
		putFlag(FLAG_H, checkOverflow(3, src_v, 1, false));
	}
}

void CPU::DEC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(dst, src_v--);

	if (src.type == OperandType::R8) {
		putFlag(FLAG_Z, sum == 0);
		setFlag(FLAG_N);
		putFlag(FLAG_H, 1 > (src_v & 0xF));
	}
}

void CPU::CP(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = dst_v - src_v;

	putFlag(FLAG_Z, sum == 0);
	setFlag(FLAG_N);
	putFlag(FLAG_H, (src & 0xF) > (dst & 0xF));
	putFlag(FLAG_C, src > dst);
}

// Bitwise Logic
void CPU::AND(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t result = src_v & dst_v;
	writeOperand(dst, result);

	putFlag(FLAG_Z, (result == 0));
	clearFlag(FLAG_N);
	setFlag(FLAG_H);
	clearFlag(FLAG_C);
}

void CPU::CPL(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(dst, ~src_v);

	setFlag(FLAG_N);
	setFlag(FLAG_H);
}

void CPU::OR(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t result = src_v | dst_v;
	writeOperand(dst, result);

	putFlag(FLAG_Z, (result == 0));
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	clearFlag(FLAG_C);
}

void CPU::XOR(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t result = src_v ^ dst_v;
	writeOperand(dst, result);

	putFlag(FLAG_Z, (result & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	clearFlag(FLAG_C);
}

// Bit Flag
void CPU::BIT(Operand src, Operand dst) {
	uint16_t bit = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t value = dst_v & (1 << bit);

	putFlag(FLAG_Z, (value == 0));
	clearFlag(FLAG_N);
	setFlag(FLAG_H);
}

void CPU::SET(Operand src, Operand dst) {
	uint16_t bit = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	dst_v |= (1 << bit);
	writeOperand(dst, dst_v);
}

void CPU::RES(Operand src, Operand dst) {
	uint16_t bit = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	dst_v &= ~(1 << bit);
	writeOperand(dst, dst_v);
}

// Bit Shift

void CPU::RL(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = (src_v << 1) | (uint8_t)getFlag(FLAG_C);
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);
}

void CPU::RLA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = (src_v << 1) | (uint8_t)getFlag(FLAG_C);
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);
}

void CPU::RLC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = src_v << 1;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);
}

void CPU::RLCA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = src_v << 1;
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);
}

void CPU::RR(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * getFlag(FLAG_C);
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
}

void CPU::RRA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * getFlag(FLAG_C);
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
}

void CPU::RRC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * (src_v & 1);
	putFlag(FLAG_C, wrap);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
}

void CPU::RRCA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * (src_v & 1);
	putFlag(FLAG_C, wrap);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
}

void CPU::SLA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = (src_v << 1);
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);
}

void CPU::SRA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t hold = src_v & 0x80;
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1) | hold;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
}

void CPU::SRL(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1);
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
}

void CPU::SWAP(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint8_t low = src_v & 0x0F;
	src_v = (src_v >> 4) | (low << 4);
	
	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	clearFlag(FLAG_C);
}

// Jumps and Subroutines
void CPU::CALL(Operand src, Operand dst) {
	uint16_t call_addr = readOperand(src);
	if (dst.type == OperandType::COND && !readOperand(dst)) {
		return;
	}

	SP()--;
	write(SP(), (PC() + 1,0xFF00) >> 8);
	SP()--;
	write(SP(), (PC() + 1, 0xFF));

	PC() = call_addr;
}

void CPU::JP(Operand src, Operand dst) {
	uint16_t jp_addr = readOperand(src);
	if (dst.type == OperandType::COND && !readOperand(dst)) {
		return;
	}

	PC() = jp_addr;
}

void CPU::JR(Operand src, Operand dst) {

}

void CPU::RET(Operand src, Operand dst) {
	if (src.type == OperandType::COND && !readOperand(src)) {
		return;
	}

	uint16_t low = read(SP());
	SP()++;
	uint16_t high = read(SP());
	SP()++;

	PC() = (high << 8) | low;
}

void CPU::RETI(Operand src, Operand dst) {

}

void CPU::RST(Operand src, Operand dst) {

}

// Carry Flag
void CPU::CCF(Operand src, Operand dst) {
	putFlag(FLAG_C, !getFlag(FLAG_C));
}

void CPU::SCF(Operand src, Operand dst) {
	setFlag(FLAG_C);
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