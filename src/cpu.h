#include <stdint.h>

union Reg16 {
	struct {
		uint8_t low;
		uint8_t high;
	};
	uint16_t reg;

	Reg16() : reg(0) {}
};

//the SM83 chip
class CPU {
public:
	CPU();
	~CPU();

private:
	Reg16 af;
		// Flag bits 7: z, 6: n, 5: h, 4:c, 3210: unused
	Reg16 bc;
	Reg16 de;
	Reg16 hl;
	uint16_t sp;
	uint16_t pc;

public:
	uint8_t&  A()  { return af.high; }
	uint16_t& AF() { return af.reg; }

	uint8_t&  B()  { return bc.high; }
	uint8_t&  C()  { return bc.low; }
	uint16_t& BC() { return bc.reg; }

	uint8_t&  D()  { return de.high; }
	uint8_t&  E()  { return de.low; }
	uint16_t& DE() { return de.reg; }

	uint8_t&  H()  { return hl.high; }
	uint8_t&  L()  { return hl.low; }
	uint16_t& HL() { return hl.reg; }

	uint16_t& SP() { return sp; }
	uint16_t& PC() { return pc; }

private:

	// First octal
	// Second octal
	//	  Similar to opcode_group()

	void decode(uint8_t opcode);
	uint8_t&  select_r8(uint8_t selector);
	uint16_t& select_r16(uint8_t selector);
	uint16_t& select_r16stk(uint8_t selector);
	uint16_t& select_r16mem(uint8_t selector);
	// cond
	// b3?
	// tgt3??
	// imm8?
	// imm16?

};