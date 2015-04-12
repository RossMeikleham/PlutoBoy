
#ifndef __DIR_BROWSE_H_
#define __DIR_BROWSE_H_
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include"mxfont.h"


struct file_node {
	char *name;
	unsigned char is_dir;
	struct file_node *next;
};

struct directory_browser {
	int cur_i,x,y,w,h,length,max_col;
	struct file_node *head;
};

extern struct file_node *ls2link(const char *dir, int *length);
extern void free_nodes(struct file_node *node);
extern void print_nodes(struct file_node *node) ;
extern void draw_browser(SDL_Surface *front, struct directory_browser *dir, struct SDL_Font *font);
extern struct file_node *skip_nodes(struct file_node *n, unsigned int i) ;
#endif
