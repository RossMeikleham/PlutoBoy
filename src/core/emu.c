#include "cpu.h"
#include "graphics.h"
#include "rom_info.h"
#include "timers.h"
#include "lcd.h"
#include "interrupts.h"
#include "mmu/memory.h"

#include "../non_core/joypad.h"
#include "../non_core/files.h"
#include "../non_core/logger.h"
#include "../non_core/debugger.h"


// Debug options
static int DEBUG = 0;
static int step_count = STEPS_OFF;
static int breakpoint = BREAKPOINT_OFF;

/* Intialize emulator with given ROM file, and
 * specify whether or not debug mode is active
 * (0 for OFF, any other value is on) 
 *
 * returns 1 if successfully initialized, 0
 * otherwise */
int init(const char *file_path, int debugger) {
    
    unsigned char buffer[MAX_FILE_SIZE];
    unsigned long size;

    //Start logger
    set_log_level(LOG_INFO);

    if (!(size = load_rom_from_file(file_path, buffer))) {
        log_message(LOG_ERROR, "failed to load ROM\n");
        return 0;
    }

    if (!load_rom(file_path, buffer, size)) {
        log_message(LOG_ERROR, "failed to initialize GB memory\n");
        return 0;
    }
    
    if (!init_gfx()) {
        log_message(LOG_ERROR, "failed to initialize graphics\n");
        return 0;
    }

    init_joypad();
    reset_cpu();
  
    if (debugger) {
        DEBUG = 1;
    }

    //Log ROM info
    
    char name_buf[100]; 
    int i;
    for(i = ROM_NAME_START; i<= ROM_NAME_END; i++) {
        name_buf[i - ROM_NAME_START] = get_mem(i);
    }
    name_buf[i] = '\0';

    log_message(LOG_INFO,"Game Title: %s\n", name_buf);
    log_message(LOG_INFO,"Licensee: %s\n", get_licensee());
    log_message(LOG_INFO,"Destination: %s\n", get_destination_code());
    log_message(LOG_INFO,"ROM size: %dKB\n",get_rom_size());
    log_message(LOG_INFO,"RAM save size: %dKB\n",get_ram_save_size());
    
    const char *c_type = get_cartridge_type();
    log_message(LOG_INFO,"Cartridge Type: %s\n",c_type != NULL ? c_type : "Unknown");

    log_message(LOG_INFO,"Gameboy Color Only Game:%s\n", is_colour_compatible() ? "Yes":"No");
    log_message(LOG_INFO,"Super Gameboy Features:%s\n", has_sgb_features() ? "Yes":"No");
    
    
    return 1;    
}

//Main Fetch-Decode-Execute loop
int run() {

    long current_cycles;
    int skip_bug = 0;
    long cycles = 0;

    if (DEBUG) {
        int flags = get_command();
        step_count = (flags & STEPS_SET) ?  get_steps() : STEPS_OFF;

        breakpoint =  (flags & BREAKPOINT_SET) ? 
            get_breakpoint() : BREAKPOINT_OFF;
    }

    for(;;) { 
        
        if (is_halted() || is_stopped()) {
            current_cycles = 4;
            update_timers(current_cycles);
            
            if (is_stopped()) {
                key_pressed();
            }
            if (is_halted()) {
                update_graphics(current_cycles);
            }
            
        } else {
            current_cycles = exec_opcode(skip_bug);
            skip_bug = 0;
        } 

        cycles += current_cycles;
        if (cycles > 100) {
            update_keys();
            cycles -= 100;
        }
        skip_bug = handle_interrupts();            
        
        if (DEBUG && step_count > 0 && --step_count == 0) {
            printf("wtf\n");
            int flags = get_command();
            step_count = (flags & STEPS_SET) ? get_steps() : STEPS_OFF;

            breakpoint =  (flags & BREAKPOINT_SET) ? 
                get_breakpoint() : BREAKPOINT_OFF;
         
        }
   }

    return 0;
}

