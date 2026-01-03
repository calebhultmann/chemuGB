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
		int (CPU::*execute)(Operand, Operand);
		Operand src;
		Operand dst;
	};

//
public:
	int remaining_cycles = 0;
	void clock();
	int executeInstruction(Instruction& curr);

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
	int LD(Operand src, Operand dst);
	int LDH(Operand src, Operand dst);
	
	// Arithmetic
	int ADC(Operand src, Operand dst);
	int ADD(Operand src, Operand dst);
	int SBC(Operand src, Operand dst);
	int SUB(Operand src, Operand dst);
	int INC(Operand src, Operand dst);
	int DEC(Operand src, Operand dst);
	int CP(Operand src, Operand dst);

	// Bitwise Logic
	int AND(Operand src, Operand dst);
	int CPL(Operand src, Operand dst);
	int OR(Operand src, Operand dst);
	int XOR(Operand src, Operand dst); 

	// Bit flag
	int BIT(Operand src, Operand dst);
	int SET(Operand src, Operand dst);
	int RES(Operand src, Operand dst);
	
	// Bit Shift
	int RL(Operand src, Operand dst);
	int RLA(Operand src, Operand dst);
	int RLC(Operand src, Operand dst);
	int RLCA(Operand src, Operand dst);
	int RR(Operand src, Operand dst);
	int RRA(Operand src, Operand dst);
	int RRC(Operand src, Operand dst);
	int RRCA(Operand src, Operand dst);
	int SLA(Operand src, Operand dst);
	int SRA(Operand src, Operand dst);
	int SRL(Operand src, Operand dst);
	int SWAP(Operand src, Operand dst);

	// Jumps and Subroutines
	int CALL(Operand src, Operand dst);
	int JP(Operand src, Operand dst);
	int JR(Operand src, Operand dst);
	int RET(Operand src, Operand dst);
	int RETI(Operand src, Operand dst);
	int RST(Operand src, Operand dst);

	// Carry Flag
	int CCF(Operand src, Operand dst);
	int SCF(Operand src, Operand dst);

	// Stack
	int POP(Operand src, Operand dst);
	int PUSH(Operand src, Operand dst);

	// Interrupt-related
	int DI(Operand src, Operand dst);
	int EI(Operand src, Operand dst);
	int HALT(Operand src, Operand dst);

	// Miscellaneous
	int DAA(Operand src, Operand dst);
	int NOP(Operand src, Operand dst);
	int STOP(Operand src, Operand dst);
	int CB(Operand src, Operand dst);
	int INV(Operand src, Operand dst);
};