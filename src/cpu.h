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
	Reg16 AF;
		// Flag bits 7: z, 6: n, 5: h, 4:c, 3210: unused
	Reg16 BC;
	Reg16 DE;
	Reg16 HL;
	uint16_t SP;
	uint16_t PC;

public:
	uint8_t&  A()  { return AF.high; }
	uint16_t& AF() { return AF.reg; }

	uint8_t&  B()  { return BC.high; }
	uint8_t&  C()  { return BC.low; }
	uint16_t& BC() { return BC.reg; }

	uint8_t&  D()  { return DE.high; }
	uint8_t&  E()  { return DE.low; }
	uint16_t& DE() { return DE.reg; }

	uint8_t&  H()  { return HL.high; }
	uint8_t&  L()  { return HL.low; }
	uint16_t& HL() { return HL.reg; }

	uint16_t& SP() { return SP; }
	uint16_t& PC() { return PC; }

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