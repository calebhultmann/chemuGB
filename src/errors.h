enum Error {
	None,
	// Cartridge Errors
	BadFile,
	InvalidROMSize,
	InvalidRAMSize,
	UnsupportedMapper,
	InvalidMapper,
	ROMNotFound,

	// Pixel Engine Errors
	NoSDLInit,
	NoSDLWindow,
	NoSDLRenderer
};