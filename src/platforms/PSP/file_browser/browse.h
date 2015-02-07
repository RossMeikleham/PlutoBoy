#ifndef BROWSE_H

/* Given a buffer of at least size 0xFF (255), select a ROM from the file system
   and store its filename in the buffer, returns 1 if successful,
   0 otherwise */
int get_gb_file(char *file_name);

#endif
