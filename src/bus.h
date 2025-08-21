// 0000 - 7FFF ROM banks
// 8000 - 9FFF VRAM (first page chrdata, second page, BG and display data)
// A000 - BFFF External RAM (cartridge)
// C000 - DFFF WRAM
// E000 - FDFF mirror of C000 - DDFF (prohibited)
// FE00 - FFFF CPU internal RAM
	// FE00 - FE9F OAM-RAM
	// FF00 - FF7F & FFFF - Specified regs and flags etc.
	// FF80 - FFFE CPU WRAM or stack RAM

// https://ia803208.us.archive.org/9/items/GameBoyProgManVer1.1/GameBoyProgManVer1.1.pdf
// https://gekkio.fi/files/gb-docs/gbctr.pdf
// https://www.copetti.org/writings/consoles/game-boy/

#include <cstdint>
#include <memory>
#include "cartridge.h"
#include "cpu.h"

class Bus {
public:
	Bus();
	~Bus();

public: // Bus devices
	CPU cpu;
	// ppu
	std::shared_ptr<Cartridge> cart;
	// vram
	// WRAM
	// internal RAM

public:
	uint8_t	read(uint16_t addr);
	void	write(uint16_t addr, uint8_t data);

public: // system functions
	// insert cartridge
	// reset
	// clock (probably?)
};