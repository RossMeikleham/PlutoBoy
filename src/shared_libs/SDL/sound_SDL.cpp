#include "../../core/sound.h"

#include <cstdio>
#include "SDL.h"

#include "audio/Gb_Apu.h"
#include "audio/Sound_Queue.h"
#include "audio/Multi_Buffer.h"



#define BUF_SIZE 8192
#define SAMPLE_RATE 44100
#define CLOCK_RATE 4194304
#define MAX_CYCLES 70000

static unsigned cycles = 0;

static Gb_Apu apu;
static Sound_Queue sound;
static Stereo_Buffer stereo_buf;
static blip_sample_t sample_buffer[BUF_SIZE];


void init_apu() {
#ifndef PSP
    if ( SDL_Init( SDL_INIT_AUDIO ) < 0 )
    {
        atexit( SDL_Quit );
    }

  
    stereo_buf.clock_rate(CLOCK_RATE); 
    stereo_buf.set_sample_rate(SAMPLE_RATE);
    apu.treble_eq(-15.0);
    stereo_buf.bass_freq(100);
    apu.set_output( stereo_buf.center(), stereo_buf.left(), stereo_buf.right() );
    sound.start(SAMPLE_RATE, 2); 
#endif    
}
    
void sound_add_cycles(unsigned c) {
#ifndef PSP  
    cycles += c;
    if (cycles >= MAX_CYCLES) {
        cycles -= MAX_CYCLES;
        end_frame();
    }
#endif
}

void write_apu(uint16_t addr, uint8_t val) {
#ifndef PSP
    apu.write_register(cycles, addr, val);
#endif
}


uint8_t read_apu(uint16_t addr) {
#ifndef PSP
   return apu.read_register(cycles, addr);
#else 
   return 0x0;
#endif
}


                           

void end_frame() {
#ifndef PSP
	    apu.end_frame(MAX_CYCLES);
        stereo_buf.end_frame(MAX_CYCLES);
			
		if (stereo_buf.samples_avail() >= BUF_SIZE) {	
		    
            size_t count = stereo_buf.read_samples(sample_buffer, BUF_SIZE );
            sound.write(sample_buffer, count );
        }
#endif
}                           


