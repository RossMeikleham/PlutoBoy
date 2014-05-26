#ifndef DISASM_H
#define DISASM_H

#include <stdio.h>

/* Send to specified stream the opcode of the instruction
 * at the specified memory location*/
void dasm_instruction(uint16_t mem, FILE const *stream);

#endif //DISASM_H
