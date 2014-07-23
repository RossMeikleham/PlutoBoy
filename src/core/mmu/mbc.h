#ifndef MBC_H
#define MBC_H

#include <stdint.h>

// Memory Bank Controller
class MBC {
private:
    void    unload_boot_rom();
    void    oam_set_mem(uint8_t addr, uint8_t val);
    uint8_t oam_get_mem(uint8_t addr);
    void    dma_transfer(uint8_t val);
    void    joypad_write(uint8_t val);
    void    io_set_mem(uint8_t addr, uint8_t val);
    uint8_t io_get_mem(uint8_t addr);

protected:
    uint16_t io_mem[0x100];
    uint16_t oam_mem[0xA0];
    uint16_t mem[0x10000 - 0x200];
    uint8_t dmg_boot_rom[0x100];

    uint8_t cartridge_start[0x100];
    uint8_t RAM_banks[4][0x2000];
    uint8_t ROM_banks[125][0x4000];
    
    unsigned current_RAM_bank;
    unsigned current_ROM_bank;
    int bank_mode;

    // Read/Write not from MBC
    void write_ordinary_mem(uint16_t addr, uint8_t val);
    uint8_t read_ordinary_mem(uint16_t addr);

public: 
    MBC(unsigned char const *file_data, unsigned long size);
    virtual void    write_mem(uint16_t addr, uint8_t val) = 0;
    virtual uint8_t read_mem(uint16_t addr) = 0;

    
};


#endif //MBC_H
