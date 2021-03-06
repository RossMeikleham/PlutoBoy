#ifndef MENU_H
#define MENU_H

// HACK, SDL2 on Vita doesn't like being closed
// and re=opened, gives an OOM error even though we
// have plenty of memory available and all the menu artifacts were released
// TO get around this we re-use the screen from the menu 
#ifdef PSVITA
extern SDL_Window *screen;  
#endif

int ui_menu(char **selected_path, int *ret_val);


#endif
