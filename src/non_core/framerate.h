#ifndef FRAMERATE_H
#define FRAMERATE_H

#define DEFAULT_FPS 59.7

//Set a framerate and start the counter
void start_framerate(float fps);


/*Adjusts speed of game to the current framerate
 *by sleeping for the required time*/
void adjust_to_framerate();


#endif
