#ifndef FRAMERATE_H
#define FRAMERATE_H

#define FPS_60 60

//Set a framerate and start the counter
void start_framerate();


/*Adjusts speed of game to the current framerate
 *by sleeping for the required time*/
void adjust_to_framerate();


#endif
