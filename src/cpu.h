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
	uint16_t sp = 0;
	uint16_t pc = 0;
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

//
public:
	void step();
	void executeInstruction(Instruction& curr);

// Public R/W Interfaces
public:
	uint8_t  read(uint16_t addr);
	void     write(uint16_t addr, uint8_t data);
	uint8_t  fetchByte();
	uint16_t fetchWord();

// Internal R/W Functions
private:
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
public:
	// Lookup Tables
	std::vector<Instruction> opcode_lookup;
	std::vector<Instruction> cb_lookup;

private:
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