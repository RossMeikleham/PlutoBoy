#ifndef __MXFONT__H_
#define __MXFONT__H_

#include<SDL.h>
#include<string.h>

struct SDL_Font {
	int mx;
	int my;
	int tcolor;
	struct letter {
		int **fnt_ptr;
	};
	struct letter letters[128];
};

extern struct SDL_Font *SDL_InitFont(const char *src);
extern void SDL_FreeFont(struct SDL_Font *m);
extern int  SDL_PrintText(struct SDL_Surface *surf, struct SDL_Font *m, int x, int y, Uint32 color, const char *src);
extern void SDL_PrintTextScaled(struct SDL_Surface *surf, struct SDL_Font *m, int x, int y, int w, int h, Uint32 color, const char *src);
extern void *lock(SDL_Surface *surf, Uint8 type);
extern void unlock(SDL_Surface *surf);
extern void setpixel(void *buff, Uint32 x, Uint32 y, Uint32 color, Uint8 type, Uint16 pitch);
extern Uint32 getpixel(SDL_Surface *surf, int x, int y, Uint8 type, Uint16 pitch) ;

#endif
