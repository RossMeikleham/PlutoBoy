#ifndef MBC_H
#define MBC_H

#include <stdint.h>

int setup_MBC(int no);

/*  Placeholders for write/read function ptrs
 *  depending on MBC mode */
typedef uint8_t (*read_MBC_ptr)(uint16_t addr);
typedef void   (*write_MBC_ptr)(uint16_t addr, uint8_t val);

read_MBC_ptr read_MBC;
write_MBC_ptr write_MBC; 



#endif //MBC_H
