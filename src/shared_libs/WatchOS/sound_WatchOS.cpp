#include "../../core/sound.h"
#include "../../core/audio/Multi_Buffer.h"
#include "../../core/audio/Gb_Apu.h"
#include "../../non_core/logger.h"
#include "sound_queue_WatchOS.h"

#include <cstdio>

#define BUF_SIZE 8192
#define SAMPLE_RATE 44100
#define CLOCK_RATE 4194304
#define MAX_CYCLES 70000

static unsigned cycles = 0;
static Gb_Apu apu;
static Stereo_Buffer stereo_buf;
static blip_sample_t sample_buffer[BUF_SIZE];

void init_apu() {
    
    stereo_buf.clock_rate(CLOCK_RATE);
    stereo_buf.set_sample_rate(SAMPLE_RATE);
    apu.treble_eq(-15.0);
    stereo_buf.bass_freq(100);
    apu.set_output( stereo_buf.center(), stereo_buf.left(), stereo_buf.right() );
    sound_queue_start(); //(SAMPLE_RATE, 2);
}
    
void sound_add_cycles(unsigned c) {
    cycles += c;
       if (cycles >= MAX_CYCLES) {
           cycles -= MAX_CYCLES;
           end_frame();
       }
}

void write_apu(uint16_t addr, uint8_t val) {
    apu.write_register(cycles, addr, val);
}


uint8_t read_apu(uint16_t addr) {
    return apu.read_register(cycles, addr);
}

void end_frame() {
    apu.end_frame(MAX_CYCLES);
    stereo_buf.end_frame(MAX_CYCLES);
              
    if (stereo_buf.samples_avail() >= BUF_SIZE) {
        size_t count = stereo_buf.read_samples(sample_buffer, BUF_SIZE );
        sound_queue_write(sample_buffer, count);
    }
}                           
