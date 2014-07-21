#ifndef DEBUGGER_H
#define DEBUGGER_H

typedef enum {NONE = 0x0, STEPS_SET = 0x1, BREAKPOINT_SET = 0x2} Command;

#define BREAKPOINT_OFF 0x0
#define STEPS_OFF 0x0

/* Get debugger command
 * and perform debugger action 
 * if steps were set and/or a breakpoint was set returns
 * the command, otherwise returns NONE*/
Command get_command();


/* Get number of steps, 0 or below
 * if stepping not occuring */
long get_steps();

// No longer stepping
void turn_steps_off();


/* Get 16bit address of breakpoint
 * result is below 0 if no breakpoint set */
long get_breakpoint(); 

//Disable breakpoint
void turn_breakpoint_off();



#endif //DEBUGGER_H
