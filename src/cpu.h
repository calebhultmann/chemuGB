#pragma once

#include <stdint.h>
#include <vector>
#include <string>

enum r8 : uint8_t {
	REG_B,
	REG_C,
	REG_D,
	REG_E,
	REG_H,
	REG_L,
	REG_HL_DATA,
	REG_A
};

enum r16 : uint8_t {
	REG_BC = 0,
	REG_DE = 1,
	REG_HL = 2,
	REG_SP = 3,

	REG_AF = 3,
	
	REG_HLI = 2,
	REG_HLD = 3,
};

enum cond : uint8_t {
	COND_NZ,
	COND_Z,
	COND_NC,
	COND_C
};

enum flag : uint8_t {
	FLAG_Z = 0b10000000,
	FLAG_N = 0b01000000,
	FLAG_H = 0b00100000,
	FLAG_C = 0b00010000
};

union Reg16 {
	struct {
		uint8_t low;
		uint8_t high;
	};
	uint16_t reg;

	Reg16() : reg(0) {}
};

class Bus;

//the SM83 chip
class CPU {
public:
	CPU();
	~CPU();

	Bus* bus = nullptr;
	void connectBus(Bus* b) { bus = b; }

// Internal Registers
private:
	Reg16 af; // 0bZNHC0000	
	Reg16 bc;
	Reg16 de;
	Reg16 hl;
	uint16_t sp;
	uint16_t pc;
	bool ime;

// External Registers
public:
	uint8_t  A()  { return af.high; }
	uint16_t AF() { return af.reg; }

	uint8_t  B()  { return bc.high; }
	uint8_t  C()  { return bc.low; }
	uint16_t BC() { return bc.reg; }

	uint8_t  D()  { return de.high; }
	uint8_t  E()  { return de.low; }
	uint16_t DE() { return de.reg; }

	uint8_t  H()  { return hl.high; }
	uint8_t  L()  { return hl.low; }
	uint16_t HL() { return hl.reg; }

	uint16_t SP() { return sp; }
	uint16_t PC() { return pc; }

// External Flag Functions
	bool getFlag(uint8_t flag);
	void setFlag(uint8_t flag);
	void clearFlag(uint8_t flag);
	void putFlag(uint8_t flag, bool value);

// Internal Operand/Instruction Structures
public:
	enum class OperandType {
		R8,
		R16,
		R16STK,
		R16MEM,
		COND,
		VEC,
		B3,

		n8,
		n16,
		a8,
		a16,

		NONE
	};

	struct Operand {
		OperandType type;
		int index;
	};

	struct Instruction {
		std::string mnemonic;
		int bytes;
		// int cycles;
		//	Only reimplement if cycles are needed, such as timing the GPU?
		void (CPU::*execute)(Operand, Operand);
		Operand src;
		Operand dst;
	};

// Public R/W Interfaces
public:
	uint8_t  read(uint16_t addr);
	void     write(uint16_t addr, uint8_t data);
	uint8_t  fetchByte();
	uint16_t fetchWord();

// Internal R/W Functions
private:
	void decode(uint8_t opcode);
	uint8_t read_r8(uint8_t selector);
	void    write_r8(uint8_t selector, uint8_t data);

	uint16_t read_r16(uint8_t selector);
	void	 write_r16(uint8_t selector, uint16_t data);
	
	uint16_t read_r16stk(uint8_t selector);
	void	 write_r16stk(uint8_t selector, uint16_t data);
		
	uint8_t read_r16mem(uint8_t selector);
	void	write_r16mem(uint8_t selector, uint8_t data);
	
	uint16_t readOperand(Operand op);
	void	 writeOperand(Operand op, uint16_t value);

	bool check_cond(uint8_t selector);

// Instructions
private:

	// Lookup Tables
	std::vector<Instruction> opcode_lookup;
	std::vector<Instruction> cb_lookup;

	// Loads
	void LD(Operand src, Operand dst);
	void LDH(Operand src, Operand dst);
	
	// Arithmetic
	void ADC(Operand src, Operand dst);
	void ADD(Operand src, Operand dst);
	void SBC(Operand src, Operand dst);
	void SUB(Operand src, Operand dst);
	void INC(Operand src, Operand dst);
	void DEC(Operand src, Operand dst);
	void CP(Operand src, Operand dst);

	// Bitwise Logic
	void AND(Operand src, Operand dst);
	void CPL(Operand src, Operand dst);
	void OR(Operand src, Operand dst);
	void XOR(Operand src, Operand dst); 

	// Bit flag
	void BIT(Operand src, Operand dst);
	void SET(Operand src, Operand dst);
	void RES(Operand src, Operand dst);
	
	// Bit Shift
	void RL(Operand src, Operand dst);
	void RLA(Operand src, Operand dst);
	void RLC(Operand src, Operand dst);
	void RLCA(Operand src, Operand dst);
	void RR(Operand src, Operand dst);
	void RRA(Operand src, Operand dst);
	void RRC(Operand src, Operand dst);
	void RRCA(Operand src, Operand dst);
	void SLA(Operand src, Operand dst);
	void SRA(Operand src, Operand dst);
	void SRL(Operand src, Operand dst);
	void SWAP(Operand src, Operand dst);

	// Jumps and Subroutines
	void CALL(Operand src, Operand dst);
	void JP(Operand src, Operand dst);
	void JR(Operand src, Operand dst);
	void RET(Operand src, Operand dst);
	void RETI(Operand src, Operand dst);
	void RST(Operand src, Operand dst);

	// Carry Flag
	void CCF(Operand src, Operand dst);
	void SCF(Operand src, Operand dst);

	// Stack
	void POP(Operand src, Operand dst);
	void PUSH(Operand src, Operand dst);

	// Interrupt-related
	void DI(Operand src, Operand dst);
	void EI(Operand src, Operand dst);
	void HALT(Operand src, Operand dst);

	// Miscellaneous
	void DAA(Operand src, Operand dst);
	void NOP(Operand src, Operand dst);
	void STOP(Operand src, Operand dst);
	void CB(Operand src, Operand dst); // Should I construct this function? Yes right?
	void INV(Operand src, Operand dst);
};

// DMB Boot rom dump:
//uint8_t dmg_boot[0x100] = {
//	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
//	0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
//	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
//	0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
//	0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
//	0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
//	0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
//	0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
//	0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xE2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
//	0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
//	0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
//	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
//	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
//	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,
//	0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
//	0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50,
//};
