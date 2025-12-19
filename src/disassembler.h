class Disassembler {
public:
    Disassembler();
    ~Disassembler();


    /*


What do I need?
 - Hashmap[address -> disassembly index]
 - Array[index -> disassembled code (string?)]

 - Disassembly function
    - start at beginning of rom/end of boot loader
    - until end of rom:
        - 

        TODO: Check for JR instructions, it is the only one that doesn't follow the rule (e8 addition)
    */
}