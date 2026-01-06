#include "cpu.h"
#include "bus.h"
#include <stdexcept>
#include <format>
#include <iostream>

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
	//   MNEMONIC              B  FUNC      SRC            DST                MNEMONIC          B  FUNC      SRC             DST                MNEMONIC            B  FUNC     SRC              DST                MNEMONIC        B  FUNC     SRC          DST                MNEMONIC              B  FUNC      SRC              DST                MNEMONIC          B  FUNC      SRC             DST                MNEMONIC            B  FUNC      SRC              DST                MNEMONIC      B  FUNC      SRC        DST
		{"NOP",                1, &c::NOP,  empt,          empt},            {"LD BC, ${:04X}", 3, &c::LD,   n16,            R16(REG_BC)},     {"LD [BC], A",       1, &c::LD,  R8(REG_A),       R16MEM(REG_BC)},  {"INC BC",       1, &c::INC, R16(REG_BC), empt},            {"INC B",              1, &c::INC,  R8(REG_B),       empt},            {"DEC B",          1, &c::DEC,  R8(REG_B),      empt},            {"LD B, ${:02X}",    2, &c::LD,   n8,              R8(REG_B)},       {"RLCA",       1, &c::RLCA, R8(REG_A), empt},
		{"LD [${:04X}], SP",   3, &c::LD,   R16(REG_SP),   a16},             {"ADD HL, BC",     1, &c::ADD,  R16(REG_BC),    R16(REG_HL)},     {"LD A, [BC]",       1, &c::LD,  R16MEM(REG_BC),  R8(REG_A)},       {"DEC BC",       1, &c::DEC, R16(REG_BC), empt},            {"INC C",              1, &c::INC,  R8(REG_C),       empt},            {"DEC C",          1, &c::DEC,  R8(REG_C),      empt},            {"LD C, ${:02X}",    2, &c::LD,   n8,              R8(REG_C)},       {"RRCA",       1, &c::RRCA, R8(REG_A), empt},
		{"STOP ${:02X}",	   2, &c::STOP, n8,            empt},            {"LD DE, ${:04X}", 3, &c::LD,   n16,            R16(REG_DE)},     {"LD [DE], A",       1, &c::LD,  R8(REG_A),       R16MEM(REG_DE)},  {"INC DE",       1, &c::INC, R16(REG_DE), empt},            {"INC D",              1, &c::INC,  R8(REG_D),       empt},            {"DEC D",          1, &c::DEC,  R8(REG_D),      empt},            {"LD D, ${:02X}",    2, &c::LD,   n8,              R8(REG_D)},       {"RLA",        1, &c::RLA,  R8(REG_A), empt},
		{"JR ${:04X}",         2, &c::JR,   n8,            empt},            {"ADD HL, DE",     1, &c::ADD,  R16(REG_DE),    R16(REG_HL)},     {"LD A, [DE]",       1, &c::LD,  R16MEM(REG_DE),  R8(REG_A)},       {"DEC DE",       1, &c::DEC, R16(REG_DE), empt},            {"INC E",              1, &c::INC,  R8(REG_E),       empt},            {"DEC E",          1, &c::DEC,  R8(REG_E),      empt},            {"LD E, ${:02X}",    2, &c::LD,   n8,              R8(REG_E)},       {"RRA",        1, &c::RRA,  R8(REG_A), empt},
		{"JR NZ, ${:04X}",     2, &c::JR,   n8,            COND(COND_NZ)},   {"LD HL, ${:04X}", 3, &c::LD,   n16,            R16(REG_HL)},     {"LD [HL+], A",      1, &c::LD,  R8(REG_A),       R16MEM(REG_HLI)}, {"INC HL",       1, &c::INC, R16(REG_HL), empt},            {"INC H",              1, &c::INC,  R8(REG_H),       empt},            {"DEC H",          1, &c::DEC,  R8(REG_H),      empt},            {"LD H, ${:02X}",    2, &c::LD,   n8,              R8(REG_H)},       {"DAA",        1, &c::DAA,  empt,      empt},
		{"JR Z, ${:04X}",      2, &c::JR,   n8,            COND(COND_Z)},    {"ADD HL, HL",     1, &c::ADD,  R16(REG_HL),    R16(REG_HL)},     {"LD A, [HL+]",      1, &c::LD,  R16MEM(REG_HLI), R8(REG_A)},       {"DEC HL",       1, &c::DEC, R16(REG_HL), empt},            {"INC L",              1, &c::INC,  R8(REG_L),       empt},            {"DEC L",          1, &c::DEC,  R8(REG_L),      empt},            {"LD L, ${:02X}",    2, &c::LD,   n8,              R8(REG_L)},       {"CPL",        1, &c::CPL,  empt,      empt},
		{"JR NC, ${:04X}",     2, &c::JR,   n8,            COND(COND_NC)},   {"LD SP, ${:04X}", 3, &c::LD,   n16,            R16(REG_SP)},     {"LD [HL-], A",      1, &c::LD,  R8(REG_A),       R16MEM(REG_HLD)}, {"INC SP",       1, &c::INC, R16(REG_SP), empt},            {"INC [HL]",           1, &c::INC,  R8(REG_HL_DATA), empt},            {"DEC [HL]",       1, &c::DEC,  R8(REG_H),      empt},            {"LD [HL], ${:02X}", 2, &c::LD,   n8,              R8(REG_HL_DATA)}, {"SCF",        1, &c::SCF,  empt,      empt},
		{"JR C, ${:04X}",      2, &c::JR,   n8,            COND(COND_C)},    {"ADD HL, SP",     1, &c::ADD,  R16(REG_SP),    R16(REG_HL)},     {"LD A, [HL-]",      1, &c::LD,  R16MEM(REG_HLD), R8(REG_A)},       {"DEC SP",       1, &c::DEC, R16(REG_SP), empt},            {"INC A",              1, &c::INC,  R8(REG_A),       empt},            {"DEC A",          1, &c::DEC,  R8(REG_A),      empt},            {"LD A, ${:02X}",    2, &c::LD,   n8,              R8(REG_A)},       {"CCF",        1, &c::CCF,  empt,      empt},

		{"LD B, B",            1, &c::LD,   R8(REG_B),     R8(REG_B)},       {"LD B, C",        1, &c::LD,   R8(REG_C),      R8(REG_B)},       {"LD B, D",          1, &c::LD,  R8(REG_D),       R8(REG_B)},       {"LD B, E",      1, &c::LD,  R8(REG_E),   R8(REG_B)},       {"LD B, H",            1, &c::LD,   R8(REG_H),       R8(REG_B)},       {"LD B, L",        1, &c::LD,   R8(REG_L),      R8(REG_B)},       {"LD B, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_B)},       {"LD B, A",    1, &c::LD,   R8(REG_A), R8(REG_B)},
		{"LD C, B",            1, &c::LD,   R8(REG_B),     R8(REG_C)},       {"LD C, C",        1, &c::LD,   R8(REG_C),      R8(REG_C)},       {"LD C, D",          1, &c::LD,  R8(REG_D),       R8(REG_C)},       {"LD C, E",      1, &c::LD,  R8(REG_E),   R8(REG_C)},       {"LD C, H",            1, &c::LD,   R8(REG_H),       R8(REG_C)},       {"LD C, L",        1, &c::LD,   R8(REG_L),      R8(REG_C)},       {"LD C, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_C)},       {"LD C, A",    1, &c::LD,   R8(REG_A), R8(REG_C)},
		{"LD D, B",            1, &c::LD,   R8(REG_B),     R8(REG_D)},       {"LD D, C",        1, &c::LD,   R8(REG_C),      R8(REG_D)},       {"LD D, D",          1, &c::LD,  R8(REG_D),       R8(REG_D)},       {"LD D, E",      1, &c::LD,  R8(REG_E),   R8(REG_D)},       {"LD D, H",            1, &c::LD,   R8(REG_H),       R8(REG_D)},       {"LD D, L",        1, &c::LD,   R8(REG_L),      R8(REG_D)},       {"LD D, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_D)},       {"LD D, A",    1, &c::LD,   R8(REG_A), R8(REG_D)},
		{"LD E, B",            1, &c::LD,   R8(REG_B),     R8(REG_E)},       {"LD E, C",        1, &c::LD,   R8(REG_C),      R8(REG_E)},       {"LD E, D",          1, &c::LD,  R8(REG_D),       R8(REG_E)},       {"LD E, E",      1, &c::LD,  R8(REG_E),   R8(REG_E)},       {"LD E, H",            1, &c::LD,   R8(REG_H),       R8(REG_E)},       {"LD E, L",        1, &c::LD,   R8(REG_L),      R8(REG_E)},       {"LD E, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_E)},       {"LD E, A",    1, &c::LD,   R8(REG_A), R8(REG_E)},
		{"LD H, B",            1, &c::LD,   R8(REG_B),     R8(REG_H)},       {"LD H, C",        1, &c::LD,   R8(REG_C),      R8(REG_H)},       {"LD H, D",          1, &c::LD,  R8(REG_D),       R8(REG_H)},       {"LD H, E",      1, &c::LD,  R8(REG_E),   R8(REG_H)},       {"LD H, H",            1, &c::LD,   R8(REG_H),       R8(REG_H)},       {"LD H, L",        1, &c::LD,   R8(REG_L),      R8(REG_H)},       {"LD H, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_H)},       {"LD H, A",    1, &c::LD,   R8(REG_A), R8(REG_H)},
		{"LD L, B",            1, &c::LD,   R8(REG_B),     R8(REG_L)},       {"LD L, C",        1, &c::LD,   R8(REG_C),      R8(REG_L)},       {"LD L, D",          1, &c::LD,  R8(REG_D),       R8(REG_L)},       {"LD L, E",      1, &c::LD,  R8(REG_E),   R8(REG_L)},       {"LD L, H",            1, &c::LD,   R8(REG_H),       R8(REG_L)},       {"LD L, L",        1, &c::LD,   R8(REG_L),      R8(REG_L)},       {"LD L, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_L)},       {"LD L, A",    1, &c::LD,   R8(REG_A), R8(REG_L)},
		{"LD [HL], B",         1, &c::LD,   R8(REG_B),     R8(REG_HL_DATA)}, {"LD [HL], C",     1, &c::LD,   R8(REG_C),      R8(REG_HL_DATA)}, {"LD [HL], D",       1, &c::LD,  R8(REG_D),       R8(REG_HL_DATA)}, {"LD [HL], E",   1, &c::LD,  R8(REG_E),   R8(REG_HL_DATA)}, {"LD [HL], H",         1, &c::LD,   R8(REG_H),       R8(REG_HL_DATA)}, {"LD [HL], L",     1, &c::LD,   R8(REG_L),      R8(REG_HL_DATA)}, {"HALT",             1, &c::HALT, empt,            empt},            {"LD [HL], A", 1, &c::LD,   R8(REG_A), R8(REG_HL_DATA)},
		{"LD A, B",            1, &c::LD,   R8(REG_B),     R8(REG_A)},       {"LD A, C",        1, &c::LD,   R8(REG_C),      R8(REG_A)},       {"LD A, D",          1, &c::LD,  R8(REG_D),       R8(REG_A)},       {"LD A, E",      1, &c::LD,  R8(REG_E),   R8(REG_A)},       {"LD A, H",            1, &c::LD,   R8(REG_H),       R8(REG_A)},       {"LD A, L",        1, &c::LD,   R8(REG_L),      R8(REG_A)},       {"LD A, [HL]",       1, &c::LD,   R8(REG_HL_DATA), R8(REG_A)},       {"LD A, A",    1, &c::LD,   R8(REG_A), R8(REG_A)},

		{"ADD A, B",           1, &c::ADD,  R8(REG_B),     R8(REG_A)},       {"ADD A, C",       1, &c::ADD,  R8(REG_C),      R8(REG_A)},       {"ADD A, D",         1, &c::ADD, R8(REG_D),       R8(REG_A)},       {"ADD A, E",     1, &c::ADD, R8(REG_E),   R8(REG_A)},       {"ADD A, H",           1, &c::ADD,  R8(REG_H),       R8(REG_A)},       {"ADD A, L",       1, &c::ADD,  R8(REG_L),      R8(REG_A)},       {"ADD A, [HL]",      1, &c::ADD,  R8(REG_HL_DATA), R8(REG_A)},       {"ADD A, A",   1, &c::ADD,  R8(REG_A), R8(REG_A)},
		{"ADC A, B",           1, &c::ADC,  R8(REG_B),     R8(REG_A)},       {"ADC A, C",       1, &c::ADC,  R8(REG_C),      R8(REG_A)},       {"ADC A, D",         1, &c::ADC, R8(REG_D),       R8(REG_A)},       {"ADC A, E",     1, &c::ADC, R8(REG_E),   R8(REG_A)},       {"ADC A, H",           1, &c::ADC,  R8(REG_H),       R8(REG_A)},       {"ADC A, L",       1, &c::ADC,  R8(REG_L),      R8(REG_A)},       {"ADC A, [HL]",      1, &c::ADC,  R8(REG_HL_DATA), R8(REG_A)},       {"ADC A, A",   1, &c::ADC,  R8(REG_A), R8(REG_A)},
		{"SUB A, B",           1, &c::SUB,  R8(REG_B),     R8(REG_A)},       {"SUB A, C",       1, &c::SUB,  R8(REG_C),      R8(REG_A)},       {"SUB A, D",         1, &c::SUB, R8(REG_D),       R8(REG_A)},       {"SUB A, E",     1, &c::SUB, R8(REG_E),   R8(REG_A)},       {"SUB A, H",           1, &c::SUB,  R8(REG_H),       R8(REG_A)},       {"SUB A, L",       1, &c::SUB,  R8(REG_L),      R8(REG_A)},       {"SUB A, [HL]",      1, &c::SUB,  R8(REG_HL_DATA), R8(REG_A)},       {"SUB A, A",   1, &c::SUB,  R8(REG_A), R8(REG_A)},
		{"SBC A, B",           1, &c::SBC,  R8(REG_B),     R8(REG_A)},       {"SBC A, C",       1, &c::SBC,  R8(REG_C),      R8(REG_A)},       {"SBC A, D",         1, &c::SBC, R8(REG_D),       R8(REG_A)},       {"SBC A, E",     1, &c::SBC, R8(REG_E),   R8(REG_A)},       {"SBC A, H",           1, &c::SBC,  R8(REG_H),       R8(REG_A)},       {"SBC A, L",       1, &c::SBC,  R8(REG_L),      R8(REG_A)},       {"SBC A, [HL]",      1, &c::SBC,  R8(REG_HL_DATA), R8(REG_A)},       {"SBC A, A",   1, &c::SBC,  R8(REG_A), R8(REG_A)},
		{"AND A, B",           1, &c::AND,  R8(REG_B),     R8(REG_A)},       {"AND A, C",       1, &c::AND,  R8(REG_C),      R8(REG_A)},       {"AND A, D",         1, &c::AND, R8(REG_D),       R8(REG_A)},       {"AND A, E",     1, &c::AND, R8(REG_E),   R8(REG_A)},       {"AND A, H",           1, &c::AND,  R8(REG_H),       R8(REG_A)},       {"AND A, L",       1, &c::AND,  R8(REG_L),      R8(REG_A)},       {"AND A, [HL]",      1, &c::AND,  R8(REG_HL_DATA), R8(REG_A)},       {"AND A, A",   1, &c::AND,  R8(REG_A), R8(REG_A)},
		{"XOR A, B",           1, &c::XOR,  R8(REG_B),     R8(REG_A)},       {"XOR A, C",       1, &c::XOR,  R8(REG_C),      R8(REG_A)},       {"XOR A, D",         1, &c::XOR, R8(REG_D),       R8(REG_A)},       {"XOR A, E",     1, &c::XOR, R8(REG_E),   R8(REG_A)},       {"XOR A, H",           1, &c::XOR,  R8(REG_H),       R8(REG_A)},       {"XOR A, L",       1, &c::XOR,  R8(REG_L),      R8(REG_A)},       {"XOR A, [HL]",      1, &c::XOR,  R8(REG_HL_DATA), R8(REG_A)},       {"XOR A, A",   1, &c::XOR,  R8(REG_A), R8(REG_A)},
		{"OR A, B",            1, &c::OR,   R8(REG_B),     R8(REG_A)},       {"OR A, C",        1, &c::OR,   R8(REG_C),      R8(REG_A)},       {"OR A, D",          1, &c::OR,  R8(REG_D),       R8(REG_A)},       {"OR A, E",      1, &c::OR,  R8(REG_E),   R8(REG_A)},       {"OR A, H",            1, &c::OR,   R8(REG_H),       R8(REG_A)},       {"OR A, L",        1, &c::OR,   R8(REG_L),      R8(REG_A)},       {"OR A, [HL]",       1, &c::OR,   R8(REG_HL_DATA), R8(REG_A)},       {"OR A, A",    1, &c::OR,   R8(REG_A), R8(REG_A)},
		{"CP A, B",            1, &c::CP,   R8(REG_B),     R8(REG_A)},       {"CP A, C",        1, &c::CP,   R8(REG_C),      R8(REG_A)},       {"CP A, D",          1, &c::CP,  R8(REG_D),       R8(REG_A)},       {"CP A, E",      1, &c::CP,  R8(REG_E),   R8(REG_A)},       {"CP A, H",            1, &c::CP,   R8(REG_H),       R8(REG_A)},       {"CP A, L",        1, &c::CP,   R8(REG_L),      R8(REG_A)},       {"CP A, [HL]",       1, &c::CP,   R8(REG_HL_DATA), R8(REG_A)},       {"CP A, A",    1, &c::CP,   R8(REG_A), R8(REG_A)},

		{"RET NZ",             1, &c::RET,  COND(COND_NZ), empt},            {"POP BC",         1, &c::POP,  R16STK(REG_BC), empt},            {"JP NZ, [${:04X}]", 3, &c::JP,  n16,             COND(COND_NZ)},   {"JP [${:04X}]", 3, &c::JP,  n16,         empt},            {"CALL NZ, [${:04X}]", 3, &c::CALL, n16,             COND(COND_NZ)},   {"PUSH BC",        1, &c::PUSH, R16STK(REG_BC), empt},            {"ADD A, ${:02X}",   2, &c::ADD,  n8,              R8(REG_A)},       {"RST $00",    1, &c::RST,  VEC(0x00), empt},
		{"RET Z",              1, &c::RET,  COND(COND_Z),  empt},            {"RET",            1, &c::RET,  empt,           empt},            {"JP Z, [${:04X}]",  3, &c::JP,  n16,             COND(COND_Z)},    {"PREFIX",       1, &c::CB,  empt,        empt},            {"CALL Z, [${:04X}]",  3, &c::CALL, n16,             COND(COND_Z)},    {"CALL [${:04X}]", 3, &c::CALL, n16,            empt},            {"ADC A, ${:02X}",   2, &c::ADC,  n8,              R8(REG_A)},       {"RST $08",    1, &c::RST,  VEC(0x08), empt},
		{"RET NC",             1, &c::RET,  COND(COND_NC), empt},            {"POP DE",         1, &c::POP,  R16STK(REG_DE), empt},            {"JP NC, [${:04X}]", 3, &c::JP,  n16,             COND(COND_NC)},   {"INVALID",      1, &c::INV, empt,        empt},            {"CALL NC, [${:04X}]", 3, &c::CALL, n16,             COND(COND_NC)},   {"PUSH DE",        1, &c::PUSH, R16STK(REG_DE), empt},            {"SUB A, ${:02X}",   2, &c::SUB,  n8,              R8(REG_A)},       {"RST $10",    1, &c::RST,  VEC(0x10), empt},
		{"RET C",              1, &c::RET,  COND(COND_C),  empt},            {"RETI",           1, &c::RETI, empt,           empt},            {"JP C, [${:04X}]",  3, &c::JP,  n16,             COND(COND_C)},    {"INVALID",      1, &c::INV, empt,        empt},            {"CALL C, [${:04X}]",  3, &c::CALL, n16,             COND(COND_C)},    {"INVALID",        1, &c::INV,  empt,           empt},            {"SBC A, ${:02X}",   2, &c::SBC,  n8,              R8(REG_A)},       {"RST $18",    1, &c::RST,  VEC(0x18), empt},
		{"LDH [$FF{:02X}], A", 2, &c::LDH,  R8(REG_A),     a8},              {"POP HL",         1, &c::POP,  R16STK(REG_HL), empt},            {"LDH [C], A",       1, &c::LDH, R8(REG_A),       R8(REG_C)},       {"INVALID",      1, &c::INV, empt,        empt},            {"INVALID",            1, &c::INV,  empt,            empt},            {"PUSH HL",        1, &c::PUSH, R16STK(REG_HL), empt},            {"AND A, ${:02X}",   2, &c::AND,  n8,              R8(REG_A)},       {"RST $20",    1, &c::RST,  VEC(0x20), empt},
		{"ADD SP, ${:02X}",    2, &c::ADD,  n8,            R16(REG_SP)},     {"JP HL",          1, &c::JP,   R16(REG_HL),    empt},            {"LD [${:04X}], A",  3, &c::LD,  R8(REG_A),       a16},             {"INVALID",      1, &c::INV, empt,        empt},            {"INVALID",            1, &c::INV,  empt,            empt},            {"INVALID",        1, &c::INV,  empt,           empt},            {"XOR A, ${:02X}",   2, &c::XOR,  n8,              R8(REG_A)},       {"RST $28",    1, &c::RST,  VEC(0x28), empt},
		{"LDH A, [$FF{:02X}]", 2, &c::LDH,  a8,            R8(REG_A)},       {"POP AF",         1, &c::POP,  R16STK(REG_AF), empt},            {"LDH A, [C]",       1, &c::LDH, R8(REG_C),       R8(REG_A)},       {"DI",           1, &c::DI,  empt,        empt},            {"INVALID",            1, &c::INV,  empt,            empt},            {"PUSH AF",        1, &c::PUSH, R16STK(REG_AF), empt},            {"OR A, ${:02X}",    2, &c::OR,   n8,              R8(REG_A)},       {"RST $30",    1, &c::RST,  VEC(0x30), empt},
		{"LD HL, SP+${:02X}",  2, &c::LD,   n8,            R16(REG_HL)},     {"LD SP, HL",      1, &c::LD,   R16(REG_HL),    R16(REG_SP)},     {"LD A, [${:04X}]",  3, &c::LD,  a16,             R8(REG_A)},       {"EI",           1, &c::EI,  empt,        empt},            {"INVALID",            1, &c::INV,  empt,            empt},            {"INVALID",        1, &c::INV,  empt,           empt},            {"CP A, ${:02X}",    2, &c::CP,   n8,              R8(REG_A)},       {"RST $38",    1, &c::RST,  VEC(0x38), empt},
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
	default:
		throw std::runtime_error("Invalid r8 selection");
	}
}

void CPU::write_r8(uint8_t selector, uint8_t data) {
	switch (selector) {
	case REG_B:
		bc.high = data; return;
	case REG_C:
		bc.low = data; return;
	case REG_D:
		de.high = data; return;
	case REG_E:
		de.low = data; return;
	case REG_H:
		hl.high = data; return;
	case REG_L:
		hl.low = data; return;
	case REG_HL_DATA:
		write(hl.reg, data); return;
	case REG_A:
		af.high = data; return;
	default:
		throw std::runtime_error("Invalid r8 selection");
	}
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
	default:
		throw std::runtime_error("Invalid r16 selection");
	}
}

void CPU::write_r16(uint8_t selector, uint16_t data) {
	switch (selector) {
	case REG_BC:
		bc.reg = data; return;
	case REG_DE:
		de.reg = data; return;
	case REG_HL:
		hl.reg = data; return;
	case REG_SP:
		sp = data; return;
	default:
		throw std::runtime_error("Invalid r16 selection");
	}
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
	default:
		throw std::runtime_error("Invalid r16stk selection");
	}
}

void CPU::write_r16stk(uint8_t selector, uint16_t data) {
	switch (selector) {
	case REG_BC:
		bc.reg = data; return;
	case REG_DE:
		de.reg = data; return;
	case REG_HL:
		hl.reg = data; return;
	case REG_AF:
		af.reg = data; return;
	default:
		throw std::runtime_error("Invalid r16stk selection");
	}
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
	default:
		throw std::runtime_error("Invalid r16mem selection");
	}
}

void CPU::write_r16mem(uint8_t selector, uint8_t data) {
	switch (selector) {
	case REG_BC:
		write(bc.reg, data); return;
	case REG_DE:
		write(de.reg, data); return;
	case REG_HLI:
		write(hl.reg++, data); return;
	case REG_HLD:
		write(hl.reg--, data); return;
	default:
		throw std::runtime_error("Invalid r16mem selection");
	}
}

bool CPU::check_cond(uint8_t selector) {
	switch (selector) {
	case COND_NZ:
		return (af.low & 0b10000000) == 0;
	case COND_Z:
		return (af.low & 0b10000000);
	case COND_NC:
		return (af.low & 0b00010000) == 0;
	case COND_C:
		return (af.low & 0b00010000);
	default:
		throw std::runtime_error("Invalid cond selection");
	}
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
	
	default: throw std::runtime_error("readOperand: Invalid operand type");
	}
}

void CPU::writeOperand(Operand op, uint16_t value) {
	switch (op.type) {
	case OperandType::R8: write_r8(op.index, value & 0xFF); return;
	case OperandType::R16: write_r16(op.index, value); return;
	case OperandType::R16STK: write_r16stk(op.index, value); return;
	case OperandType::R16MEM: write_r16mem(op.index, value & 0xFF); return;
	case OperandType::a8: write(0xFF00 | fetchByte(), value & 0xFF); return;
	case OperandType::a16: write(fetchWord(), value & 0xFF); return;
	
	default: throw std::runtime_error("writeOperand: Invalid operand type");
	}
}

bool checkOverflow(int bit, uint16_t num1, uint16_t num2, bool carry) {
	num1 >>= bit;
	num2 >>= bit;

	num1 <<= bit;
	num2 <<= bit;

	uint32_t low1 = (uint32_t)(num1 & (~num1));
	uint32_t low2 = (uint32_t)(num2 & (~num2));
	
	uint32_t sum = low1 + low2 + carry;
	return sum >> bit;
}

/*
Check off the following instructions to make sure all are working properly:

Stack manipulation instructions
LD HL,SP+e8

Interrupt-related instructions
HALT

Miscellaneous instructions
DAA
NOP
STOP
*/

void CPU::clock() {
	if (remaining_cycles == 0) {
		uint16_t instruction_addr = pc; //
		std::string instruction_str = "...";

		int opcode = fetchByte();
		Instruction curr = opcode_lookup[opcode];

		if (bus->bank == 1) {
			std::string mnemonic = curr.mnemonic; //
			int bytes = curr.bytes - 1;
			uint16_t value = 0;

			if (bytes > 0) {
				uint16_t byte_addr = instruction_addr;
				for (int i = bytes - 1; i >= 0; i--) {
					byte_addr++;
					uint16_t num = read(byte_addr);
					value |= (num << (8 * ((bytes - 1) - i)));
				}
				instruction_str = std::vformat(mnemonic, std::make_format_args(value));
			}
			else {
				instruction_str = mnemonic;
			}
			std::string output = std::format("{:04X}: {}", instruction_addr, instruction_str);
			std::cout << output << "\n";
		}

 		remaining_cycles = executeInstruction(curr);
	}

	remaining_cycles--;
}

int CPU::executeInstruction(Instruction& curr) {
	return (this->*curr.execute)(curr.src, curr.dst);
}

// Loads
int CPU::LD(Operand src, Operand dst) {
	if (dst.type == OperandType::R16 && src.type == OperandType::n8) {
		// TODO: LD HL, SP+e8 Flags (and addition)
		uint16_t offset = readOperand(src);
		uint16_t addr = sp + static_cast<int8_t>(offset);
		writeOperand(dst, addr);
		return 12;
	}
	uint16_t src_v = readOperand(src);
	writeOperand(dst, src_v);

	if (src.type == OperandType::R16MEM || dst.type == OperandType::R16MEM) {
		return 8;
	}

	if (src.type == OperandType::a16) {
		return 16;
	}

	if (src.type == OperandType::n16) {
		return 12;
	}

	if (dst.type == OperandType::a16) {
		return 20;
	}

	if (dst.type == OperandType::R16) {
		return 8;
	}

	if (src.type == OperandType::n8) {
		if (dst.index == REG_HL_DATA) {
			return 12;
		}
		return 8;
	}

	if (src.index == REG_HL_DATA || dst.index == REG_HL_DATA) {
		return 8;
	}

	return 4;
}

int CPU::LDH(Operand src, Operand dst) {
	uint16_t src_v;

	if (src.type == OperandType::R8 && src.index == REG_C) {
		uint16_t c = readOperand(src);
		src_v = read(0xFF00 | c);
	}
	else {
		src_v = readOperand(src);
	}

	if (dst.type == OperandType::R8 && dst.index == REG_C) {
		uint16_t c = readOperand(dst);
		write(0xFF00 | c, (uint8_t)src_v);
	}
	else {
		writeOperand(dst, src_v);
	}

	if (src.type == OperandType::a8 || dst.type == OperandType::a8) {
		return 12;
	}

	return 8;
}

// Arithmetic
int CPU::ADC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = src_v + dst_v + getFlag(FLAG_C);
	writeOperand(dst, sum);
	
	putFlag(FLAG_Z, (sum == 0));
	clearFlag(FLAG_N);
	putFlag(FLAG_H, checkOverflow(3, src_v, dst_v, getFlag(FLAG_C)));
	putFlag(FLAG_C, checkOverflow(7, src_v, dst_v, getFlag(FLAG_C)));

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

int CPU::ADD(Operand src, Operand dst) {
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
		putFlag(FLAG_Z, !isSP && ((sum & 0xFF) == 0));
		clearFlag(FLAG_N);
		putFlag(FLAG_H, checkOverflow(3, src_v, dst_v, false));
		putFlag(FLAG_C, checkOverflow(7, src_v, dst_v, false));
	}

	if (src.type == OperandType::R16) {
		return 8;
	}

	if (dst.type == OperandType::R16) {
		return 16;
	}

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

int CPU::SBC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = dst_v - src_v - getFlag(FLAG_C);
	writeOperand(dst, sum);

	putFlag(FLAG_Z, sum == 0);
	setFlag(FLAG_N);
	putFlag(FLAG_H, (src_v & 0xF) > ((dst_v + getFlag(FLAG_C)) & 0xF));
	putFlag(FLAG_C, src_v > (dst_v + getFlag(FLAG_C)));

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

int CPU::SUB(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = dst_v - src_v;
	writeOperand(dst, sum);

	putFlag(FLAG_Z, sum == 0);
	setFlag(FLAG_N);
	putFlag(FLAG_H, (src_v & 0xF) > (dst_v & 0xF));
	putFlag(FLAG_C, src_v > dst_v);

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

int CPU::INC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(src, ++src_v);

	if (src.type == OperandType::R8) {
		putFlag(FLAG_Z, (src_v & 0xFF) == 0);
		clearFlag(FLAG_N);
		putFlag(FLAG_H, checkOverflow(3, src_v, 1, false));
	}

	if (src.type == OperandType::R16) {
		return 8;
	}

	if (src.index == REG_HL_DATA) {
		return 12;
	}

	return 4;
}

int CPU::DEC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(src, --src_v);

	if (src.type == OperandType::R8) {
		putFlag(FLAG_Z, (src_v & 0xFF) == 0);
		setFlag(FLAG_N);
		putFlag(FLAG_H, 1 > (src_v & 0xF));
	}

	if (src.type == OperandType::R16) {
		return 8;
	}

	if (src.index == REG_HL_DATA) {
		return 12;
	}

	return 4;
}

int CPU::CP(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t sum = dst_v - src_v;

	putFlag(FLAG_Z, sum == 0);
	setFlag(FLAG_N);
	putFlag(FLAG_H, (src_v & 0xF) > (dst_v & 0xF));
	putFlag(FLAG_C, src_v > dst_v);

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

// Bitwise Logic
int CPU::AND(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t result = src_v & dst_v;
	writeOperand(dst, result);

	putFlag(FLAG_Z, (result == 0));
	clearFlag(FLAG_N);
	setFlag(FLAG_H);
	clearFlag(FLAG_C);

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

int CPU::CPL(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	writeOperand(dst, ~src_v);

	setFlag(FLAG_N);
	setFlag(FLAG_H);

	return 4;
}

int CPU::OR(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t result = src_v | dst_v;
	writeOperand(dst, result);

	putFlag(FLAG_Z, (result == 0));
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	clearFlag(FLAG_C);

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

int CPU::XOR(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t result = src_v ^ dst_v;
	writeOperand(dst, result);

	putFlag(FLAG_Z, (result & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	clearFlag(FLAG_C);

	if (src.index == REG_HL_DATA || src.type == OperandType::n8) {
		return 8;
	}

	return 4;
}

// Bit Flag
int CPU::BIT(Operand src, Operand dst) {
	uint16_t bit = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	uint16_t value = dst_v & (1 << bit);

	putFlag(FLAG_Z, (value == 0));
	clearFlag(FLAG_N);
	setFlag(FLAG_H);

	if (src.index == REG_HL_DATA) {
		return 12;
	}

	return 8;
}

int CPU::SET(Operand src, Operand dst) {
	uint16_t bit = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	dst_v |= (1 << bit);
	writeOperand(dst, dst_v);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::RES(Operand src, Operand dst) {
	uint16_t bit = readOperand(src);
	uint16_t dst_v = readOperand(dst);
	dst_v &= ~(1 << bit);
	writeOperand(dst, dst_v);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

// Bit Shift

int CPU::RL(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = (src_v << 1) | (uint8_t)getFlag(FLAG_C);
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::RLA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = (src_v << 1) | (uint8_t)getFlag(FLAG_C);
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);

	return 4;
}

int CPU::RLC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = src_v << 1;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::RLCA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = src_v << 1;
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);

	return 4;
}

int CPU::RR(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * getFlag(FLAG_C);
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::RRA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * getFlag(FLAG_C);
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);

	return 4;
}

int CPU::RRC(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * (src_v & 1);
	putFlag(FLAG_C, wrap);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::RRCA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t wrap = 0x80 * (src_v & 1);
	putFlag(FLAG_C, wrap);
	src_v = (src_v >> 1) | wrap;
	writeOperand(src, src_v);

	clearFlag(FLAG_Z);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);

	return 4;
}

int CPU::SLA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	src_v = (src_v << 1);
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	putFlag(FLAG_C, src_v & 0xFF00);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::SRA(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint16_t hold = src_v & 0x80;
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1) | hold;
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::SRL(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	putFlag(FLAG_C, src_v & 1);
	src_v = (src_v >> 1);
	writeOperand(src, src_v);

	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

int CPU::SWAP(Operand src, Operand dst) {
	uint16_t src_v = readOperand(src);
	uint8_t low = src_v & 0x0F;
	src_v = (src_v >> 4) | (low << 4);
	
	putFlag(FLAG_Z, (src_v & 0xFF) == 0);
	clearFlag(FLAG_N);
	clearFlag(FLAG_H);
	clearFlag(FLAG_C);

	if (src.index == REG_HL_DATA) {
		return 16;
	}

	return 8;
}

// Jumps and Subroutines
int CPU::CALL(Operand src, Operand dst) {
	uint16_t call_addr = readOperand(src);
	if (dst.type == OperandType::COND && !readOperand(dst)) {
		return 12;
	}

	sp--;
	write(sp, (pc & 0xFF00) >> 8);
	sp--;
	write(sp, (pc & 0xFF));

	pc = call_addr;
	return 24;
}

int CPU::JP(Operand src, Operand dst) {
	uint16_t jp_addr = readOperand(src);
	if (dst.type == OperandType::COND && !readOperand(dst)) {
		return 12;
	}

	pc = jp_addr;
	if (src.type == OperandType::a16) {
		return 16;
	}
	return 4;
}

int CPU::JR(Operand src, Operand dst) {
	uint16_t jr_offset = readOperand(src);
	if (dst.type == OperandType::COND && !readOperand(dst)) {
		return 8;
	}

	pc = pc + static_cast<int8_t>(jr_offset);
	return 12;
}

int CPU::RET(Operand src, Operand dst) {
	if (src.type == OperandType::COND && !readOperand(src)) {
		return 8;
	}

	uint16_t low = read(sp);
	sp++;
	uint16_t high = read(sp);
	sp++;

	pc = (high << 8) | low;
	if (src.type == OperandType::COND) {
		return 20;
	}

	return 16;
}

int CPU::RETI(Operand src, Operand dst) {
	uint16_t low = read(sp);
	sp++;
	uint16_t high = read(sp);
	sp++;

	ime = true;
	pc = (high << 8) | low;
	return 16;
}

int CPU::RST(Operand src, Operand dst) {
	uint16_t rst_addr = readOperand(src);

	sp--;
	write(sp, (pc, 0xFF00) >> 8);
	sp--;
	write(sp, (pc, 0xFF));

	pc = rst_addr;

	return 16;
}

// Carry Flag
int CPU::CCF(Operand src, Operand dst) {
	putFlag(FLAG_C, !getFlag(FLAG_C));
	return 4;
}

int CPU::SCF(Operand src, Operand dst) {
	setFlag(FLAG_C);
	return 4;
}

// Stack
int CPU::POP(Operand src, Operand dst) {
	uint16_t low = read(sp);
	sp++;
	uint16_t high = read(sp);
	sp++;
	writeOperand(src, (high << 8) | low);
	return 12;
}

int CPU::PUSH(Operand src, Operand dst) {
	uint16_t reg = readOperand(src);
	sp--;
	write(sp, (reg & 0xFF00) >> 8);
	sp--;
	write(sp, reg & 0xFF);
	return 16;
}

// Interrupt-related
int CPU::DI(Operand src, Operand dst) {
	ime = false;
	return 4;
}

int CPU::EI(Operand src, Operand dst) {
	ime = true;
	return 4;
}

int CPU::HALT(Operand src, Operand dst) {
	return 4;
}

// Miscellaneous
int CPU::DAA(Operand src, Operand dst) {
	return 4;
}

int CPU::NOP(Operand src, Operand dst) {
	return 4;
}

int CPU::STOP(Operand src, Operand dst) {
	return 4;
}

int CPU::CB(Operand src, Operand dst) {
	int opcode = fetchByte();
	Instruction curr = cb_lookup[opcode];
	return executeInstruction(curr) + 4;
}

int CPU::INV(Operand src, Operand dst) {
	// Brick the console
	while (true) {

	}
	return 0;
}