#include "mbc.h"

class MBC0 : public MBC {
public:
    MBC0 (unsigned char const *file_data, unsigned long size) : 
        MBC(file_data, size) {}

    virtual void    write_mem(uint16_t addr, uint8_t val);
    virtual uint8_t read_mem(uint16_t addr);
};
