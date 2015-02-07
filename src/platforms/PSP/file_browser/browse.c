#include<stdio.h>
#include "dirbrowse.h"
#include<SDL.h>

SDL_Surface *front = 0;
struct SDL_Font *the_font = 0;
SDL_Joystick *stick;

int get_gb_file(char *file_name) {

	struct directory_browser b;
	SDL_Event e;
	int active = 1;
	SDL_Surface *bg = 0;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
		return -1;

	if(!(front = SDL_SetVideoMode(640,480,0,0))) 
		return -1;

	stick = SDL_JoystickOpen(0);
	SDL_JoystickEventState(SDL_ENABLE);


	the_font = SDL_InitFont("Resources/arial.mxf");
	SDL_FillRect(front, 0, 0);
	SDL_PrintText(front, the_font, 0,0,SDL_MapRGB(front->format, 255, 0, 0), "Loading...");
	SDL_UpdateRect(front, 0,0,640,480);

	//bg = SDL_LoadBMP("background.bmp");


	fprintf(stderr, "starting to browse\n");
	b.head = ls2link("./",&b.length);
	b.x = 10;
	b.y = 10;
	b.w = 620;
	b.h = 420;
	b.cur_i = 0;
	b.max_col = 10;
    int file_selected = 0;

	while(active == 1) {

		char str[255];
		SDL_FillRect(front, 0,0);
		SDL_BlitSurface(bg, 0, front, 0);
		draw_browser(front, &b, the_font);
		sprintf(str, "Index: %d Total: %d", b.cur_i+1, b.length);
		SDL_PrintText(front, the_font, 420,10, SDL_MapRGB(front->format, 255, 255, 255), str);
		if(SDL_PollEvent(&e)) {
			switch(e.type) {
				case SDL_QUIT:
					active = 0;
					break;
				case SDL_JOYBUTTONDOWN:
					{
						switch(e.jbutton.button) {
						case 8: // move up
							if(b.cur_i > 0)
								b.cur_i--;
							break;
						case 6:// move down
							if(b.cur_i+1 < b.length)
								b.cur_i++;
							break;
						case 7:// move left
							if(b.cur_i+1-b.max_col > 0)
								b.cur_i -= b.max_col;
							break;
						case 9://move right
							if(b.cur_i+b.max_col < b.length)
								b.cur_i += b.max_col;
							break;
                        case 10:
                            active = 0;
                            break;
						case 0:
						case 1:
						case 2:
						case 3:
							{
								struct file_node *pnode = skip_nodes(b.head->next, b.cur_i);
								if(pnode != 0 && pnode->is_dir == 1) {
									char buf[1000];
									sprintf(buf,"./%s/", pnode->name);
									struct file_node *new_nodes = ls2link(buf, &b.length);
									free_nodes(b.head);
									b.cur_i = 0;
									b.head = new_nodes;
								}
								else if(pnode != 0 && pnode->is_dir == 0) { // its a file
                                    strncpy(file_name, pnode->name, 0xFF); 
                                    file_selected = 1;
                                    active = 0;
								}
							}

							break;
						}
					}
					break;
			}
		}

		SDL_UpdateRect(front, 0,0,640,480);
	}

	
	free_nodes(b.head);
	SDL_FreeSurface(bg);
	SDL_FreeFont(the_font);
	SDL_FreeSurface(front);
	SDL_JoystickClose(stick);
	SDL_Quit();

	return file_selected;
}
