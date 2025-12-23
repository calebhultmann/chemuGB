enum Error {
	None,
	// Cartridge Errors
	BadFile,
	InvalidROMSize,
	InvalidRAMSize,
	UnsupportedMapper,
	InvalidMapper,
	ROMNotFound
};