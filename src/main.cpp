#include "cpu.h"
#include <iostream>

int main(int argc, char** argv) {
	CPU cpu;
	cpu.AF.reg = 0x00FF;
	cpu.AF.reg++;
	std::cout << "Reg: ";
	std::cout << (int)cpu.AF.reg;
	/*std::cout << "High: ";
	std::cout << (int)cpu.AF.high;
	std::cout << ", Low: ";
	std::cout << (int)cpu.AF.low;*/
	return 0;
}