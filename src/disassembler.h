#include <string>
#include <map>
#include <vector>
#include "cpu.h"

class Disassembler {
public:
    Disassembler(CPU* host);
    ~Disassembler();

    CPU* cpu = nullptr;

    void disassembleROM(std::vector<std::vector<uint8_t>>);

private:
    std::map<std::pair<int, int>, std::string> disassembled;


    /*


What do I need?
 - Hashmap[address -> disassembly index]
 TODO: Draw out implementation for hashmap. How to handle multiple banks? Possible array of maps? Don't want to iterate from one bank to the next. Is it possible to just iterate until addr > ROM_BANK_SIZE instead of iter.end()?
 - Array[index -> disassembled code (string?)]

 - Disassembly function
    - start at beginning of rom/end of boot loader
    - until end of rom:
        -

        TODO: Check for JR instructions, it is the only one that doesn't follow the rule (e8 addition)
    */
};