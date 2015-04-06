#include "cpu.h"
#include "graphics.h"
#include "rom_info.h"
#include "timers.h"
#include "lcd.h"
#include "interrupts.h"
#include "mmu/memory.h"
#include "sound.h"
#include "emu.h"
#include "serial_io.h"
#include <stdio.h>

#include "../non_core/joypad.h"
#include "../non_core/files.h"
#include "../non_core/logger.h"
#include "../non_core/debugger.h"


// Debug options
int DEBUG = 0;
int step_count = STEPS_OFF;
int breakpoint = BREAKPOINT_OFF;

/* Intialize emulator with given ROM file, and
 * specify whether or not debug mode is active
 * (0 for OFF, any other value is on) 
 *
 * returns 1 if successfully initialized, 0
 * otherwise */
int init(const char *file_path, int debugger, int dmg_mode, ClientOrServer cs) {

    unsigned char buffer[MAX_FILE_SIZE];
    unsigned long size;

    //Start logger
    set_log_level(LOG_INFO);
    
    log_message(LOG_INFO, "About to open file %s\n", file_path);

    if (!(size = load_rom_from_file(file_path, buffer))) {
        log_message(LOG_ERROR, "failed to load ROM\n");
        return 0;
    }
    log_message(LOG_INFO, "File loaded %s\n", file_path);

    if (!load_rom(file_path, buffer, size, dmg_mode)) {
        log_message(LOG_ERROR, "failed to initialize GB memory\n");
        return 0;
    }
    
    if (!init_gfx()) {
        log_message(LOG_ERROR, "failed to initialize graphics\n");
        return 0;
    }

    if (!setup_serial_io(cs, 5000)) {
        log_message(LOG_INFO, "No client or server created\n");
    }
    init_joypad();
    init_apu(); // Initialize sound
    reset_cpu();
    
     
    if (debugger) {
        DEBUG = 1;
    }

    cgb_features = is_colour_compatible() || is_colour_only();
    
    //Log ROM info
    char name_buf[100]; 
    int i;
    for(i = ROM_NAME_START; i <= ROM_NAME_END; i++) {
        name_buf[i - ROM_NAME_START] = get_mem(i);
    }
    name_buf[i - ROM_NAME_START] = '\0';

    log_message(LOG_INFO,"Game Title: %s\n", name_buf);
    log_message(LOG_INFO,"Licensee: %s\n", get_licensee());
    log_message(LOG_INFO,"Destination: %s\n", get_destination_code());
    log_message(LOG_INFO,"ROM size: %dKB\n",get_rom_size());
    log_message(LOG_INFO,"RAM save size: %dKB\n",get_ram_save_size());
    
    const char *c_type = get_cartridge_type();
    log_message(LOG_INFO,"Cartridge Type: %s\n",c_type != NULL ? c_type : "Unknown");

    
    log_message(LOG_INFO, "Has Gameboy Color features: %s\n", is_colour_compatible() || is_colour_only() ? "Yes":"No");
    log_message(LOG_INFO,"Gameboy Color Only Game:%s\n", is_colour_only() ? "Yes":"No");
    log_message(LOG_INFO,"Super Gameboy Features:%s\n", has_sgb_features() ? "Yes":"No");
    
    
    return 1;    
}


static long current_cycles;
static int skip_bug = 0;
static long cycles = 0;    


void add_current_cycles(unsigned c) {
    cycles += c;
    update_all_cycles(c);
}


// Draws one frame then returns
void run_one_frame() {
    frame_drawn = 0;

    while (!frame_drawn) {
        if (halted || stopped) {

            current_cycles = 4;
            update_timers(current_cycles);
            sound_add_cycles(current_cycles);
            inc_serial_cycles(current_cycles);
       
            // If Key pressed in "stop" mode, then gameboy is "unstopped" 
            if (stopped) {
                if(key_pressed()) {
                    stopped = 0;
                }
            }
            if (halted) {
                update_graphics(current_cycles);
            }
        }
        else if (!(halted || stopped)) {
            current_cycles = 0;
            current_cycles += exec_opcode(skip_bug);
               
        }

        cycles += current_cycles;
        if (cycles > 15000) {
            update_keys();
            cycles = 0;
        }
        skip_bug = handle_interrupts();

        if (DEBUG && step_count > 0 && --step_count == 0) {
            int flags = get_command();
            step_count = (flags & STEPS_SET) ? get_steps() : STEPS_OFF;
        }
    }
    
}

void setup_debug() {
    if (DEBUG) {
        int flags = get_command();
        step_count = (flags & STEPS_SET) ?  get_steps() : STEPS_OFF;

        breakpoint =  (flags & BREAKPOINT_SET) ? 
            get_breakpoint() : BREAKPOINT_OFF;
    }


}
    
void run() {
    setup_debug();
    for(;;) {run_one_frame();} 
}
        




