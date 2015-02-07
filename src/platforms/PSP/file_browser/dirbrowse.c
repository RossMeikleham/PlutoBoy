#include"dirbrowse.h"

struct file_node *ls2link(const char *dir, int *length) {

	struct file_node *fn = 0,*nhead = 0;
	struct dirent *e = 0;
	int len = 0;
	*length = 0;

	DIR *d;
	struct stat sb;
	fn = (struct file_node*)malloc(sizeof(struct file_node));
	fn->name = 0;
	fn->next = 0;
	nhead = fn;
	
	if(!( d = opendir(dir))) {
		fprintf(stderr,"Invalid Directory\n");
		return 0;
	}

	chdir(dir);

	while ((e = readdir(d)) != 0) {
		fn->next = (struct file_node*)malloc(sizeof(struct file_node));
		fn = fn->next;
		fn->is_dir = 0;
		fn->next = 0;

		stat(e->d_name, &sb);
		if(S_ISDIR(sb.st_mode)) {
			fn->is_dir = 1;
		}
		
		fn->name = strdup(e->d_name);
		len++;
	}

	closedir(d);
	
	*length = len;
	return nhead;
}

void free_nodes(struct file_node *node) {
	if(node->next)
		free_nodes(node->next);

	if(node) {
		if(node->name)
			free(node->name);
		free(node);
	}
}

void print_nodes(struct file_node *node) {

	if(node->name)
	printf("%s\n", node->name);

	if(node->next)
		print_nodes(node->next);
}

struct file_node *skip_nodes(struct file_node *n, unsigned int i) {

	unsigned int z;
	struct file_node *ptr = n;
	for(z = 0; z < i; z++)
		ptr = ptr->next;

	return ptr;
}

extern void draw_browser(SDL_Surface *front, struct directory_browser *dir, struct SDL_Font *font) {

	int i;
	int cur_y = 0;
	struct file_node *nptr;

	SDL_Rect rc = { dir->x, dir->y, dir->w, dir->h };
	SDL_FillRect(front, &rc, 0); 
	cur_y = dir->y;

	nptr = dir->head->next;
	nptr = skip_nodes(dir->head->next, dir->cur_i);
	SDL_PrintText(front, font, dir->x, dir->y, SDL_MapRGB(front->format, 255, 0,0), "=)>");
	for(i = 0; i < dir->max_col; i++) {

		if(nptr->name != 0) {
			char dbuf[1000];
			
			if(nptr->is_dir == 1) 
				sprintf(dbuf,"[%s]", nptr->name);
			else
				sprintf(dbuf,"%s", nptr->name);

				SDL_PrintText(front, font, dir->x+35, cur_y, SDL_MapRGB(front->format, rand()%255,rand()%255,rand()%255), dbuf);

		}
		cur_y += 27;

		if(nptr->next)
			nptr = nptr->next;
		else
			break;

	}

}
