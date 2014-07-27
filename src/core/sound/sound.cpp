#include "sound.h"
#include "blaarg_sound/Audio.h"
#include <cstdio>
#include "SDL/SDL.h"

AUDIO aud;

void init_apu() {
    aud.Initialize();        
}
    

void write_apu(uint16_t addr, uint8_t val) {
    aud.WriteRegister(addr, val);
}


uint8_t read_apu(uint16_t addr) {
   return  aud.ReadRegister(addr);
}


                           

void end_frame() {
    aud.EndFrame();
}                           


