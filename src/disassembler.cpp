#include "disassembler.h"
#include <format>
#include <iostream>

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

			if (bank_index == 0 && instruction_index < 0x150) {
				instruction_str = "...";
			}
			else {
				if (bank_index == 1) {
					std::cout << "";
				}
				CPU::Instruction curr = cpu->opcode_lookup.at(opcode);
				std::string mnemonic = curr.mnemonic;
				int bytes = curr.bytes - 1;
				uint16_t value = 0;
				
				if (bytes > 0) {
					for (int i = bytes - 1; i >= 0; i--) {
						uint16_t num = 0;
						if (++instruction_index < bank.size()) {
							num = bank.at(instruction_index);
						}
						value |= (num << (8 * ((bytes - 1) - i)));
					}
					instruction_str = std::vformat(mnemonic, std::make_format_args(value));
				}
				else {
					instruction_str = mnemonic;
				}
			}
			std::string output = std::format("Bank: {}, Instruction: {:0X}, Str: {}", bank_index, curr_index, instruction_str);
			std::cout << output << "\n";
			disassembled.insert({ std::pair{ bank_index, curr_index }, instruction_str });
		}
	}
}