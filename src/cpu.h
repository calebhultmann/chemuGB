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

	Reg16 AF;
		// Flag bits 7: z, 6: n, 5: h, 4:c, 3210: unused
	Reg16 BC;
	Reg16 DE;
	Reg16 HL;
	uint16_t SP;
	uint16_t PC;
};