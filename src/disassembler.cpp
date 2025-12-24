#include "disassembler.h"
#include <format>

Disassembler::Disassembler(CPU* host) {
	cpu = host;
}

Disassembler::~Disassembler() {

}

void Disassembler::disassembleROM(std::vector<std::vector<uint8_t>> banks) {
	for (int bank_index = 0; bank_index < banks.size(); bank_index++) {
		std::vector<uint8_t> bank = banks.at(bank_index);

		for (int instruction_index = 0; instruction_index < bank.size(); instruction_index++) {
			int curr_index = instruction_index;
			uint8_t opcode = bank.at(instruction_index);

			std::string instruction_str = "";

			if (bank_index == 1 && instruction_index < 0x150) {
				instruction_str = "...";
			}
			else {
				CPU::Instruction curr = cpu->opcode_lookup.at(opcode);
				instruction_str = curr.mnemonic;
				int bytes = curr.bytes;
				uint16_t value = 0;
				
				
				
				
				for (int i = bytes - 1; i >= 0; i--) {
					uint16_t num = 0;
					if (++instruction_index < bank.size()) {
						num = bank.at(++instruction_index);
					}
					value | num << (8 * i);
				}

				instruction_str = std::format(instruction_str, value);
			}

			disassembled.insert({ std::pair{ bank_index, curr_index }, instruction_str });
		}
	}
}