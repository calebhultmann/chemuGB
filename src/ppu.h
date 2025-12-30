#include <cstdint>

class PPU
{
public:
	PPU();
	~PPU();


	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);
	uint8_t vram[0x2000];
	uint8_t oam[0xA0];

};