#include "../../../non_core/filebrowser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// "Desktop" i.e. Windows/OSX/Linux

//#ifdef POSIX
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
//#endif

#ifdef PSVITA
#define PATH_MAX 4096
#endif

// DevKitPro doesn't implement this for some reason :/
//#ifdef __SWITCH__
#define MAX_READLINKS 32

#ifdef __STDC__
char *realpath_switch(const char *path, char resolved_path[])
#else
char *realpath(path, resolved_path)
const char *path;
char resolved_path[];
#endif
{
	char copy_path[PATH_MAX];
	char link_path[PATH_MAX];
	char got_path[PATH_MAX];
	char *new_path = got_path;
	char *max_path;
	int readlinks = 0;
	int n;

	if (path == NULL) {
		errno = EINVAL;
		return NULL;
	}
	if (*path == '\0') {
		errno = ENOENT;
		return NULL;
	}
	/* Make a copy of the source path since we may need to modify it. */
	if (strlen(path) >= PATH_MAX - 2) {
		errno = ENAMETOOLONG;
		return NULL;
	}
	strcpy(copy_path, path);
	path = copy_path;
	max_path = copy_path + PATH_MAX - 2;
	/* If it's a relative pathname use getcwd for starters. */
	if (*path != '/') {
		/* Ohoo... */
		getcwd(new_path, PATH_MAX - 1);
		new_path += strlen(new_path);
		if (new_path[-1] != '/')
			*new_path++ = '/';
	} else {
		*new_path++ = '/';
		path++;
	}
	/* Expand each slash-separated pathname component. */
	while (*path != '\0') {
		/* Ignore stray "/". */
		if (*path == '/') {
			path++;
			continue;
		}
		if (*path == '.') {
			/* Ignore ".". */
			if (path[1] == '\0' || path[1] == '/') {
				path++;
				continue;
			}
			if (path[1] == '.') {
				if (path[2] == '\0' || path[2] == '/') {
					path += 2;
					/* Ignore ".." at root. */
					if (new_path == got_path + 1)
						continue;
					/* Handle ".." by backing up. */
					while ((--new_path)[-1] != '/');
					continue;
				}
			}
		}
		/* Safely copy the next pathname component. */
		while (*path != '\0' && *path != '/') {
			if (path > max_path) {
				errno = ENAMETOOLONG;
				return NULL;
			}
			*new_path++ = *path++;
		}
/*#ifdef S_IFLNK
		// Protect against infinite loops. 
		if (readlinks++ > MAX_READLINKS) {
			errno = ELOOP;
			return NULL;
		}
		// See if latest pathname component is a symlink.
		*new_path = '\0';
		n = readlink(got_path, link_path, PATH_MAX - 1);
		if (n < 0) {
			// EINVAL means the file exists but isn't a symlink. 
			if (errno != EINVAL) {
				// Make sure it's null terminated.
				*new_path = '\0';
				strcpy(resolved_path, got_path);
				return NULL;
			}
		} else {
			// Note: readlink doesn't add the null byte.
			link_path[n] = '\0';
			if (*link_path == '/')
				// Start over for an absolute symlink. 
				new_path = got_path;
			else
				// Otherwise back up over this component.
				while (*(--new_path) != '/');
			// Safe sex check. 
			if (strlen(path) + n >= PATH_MAX - 2) {
				errno = ENAMETOOLONG;
				return NULL;
			}
			// Insert symlink contents into path.
			strcat(link_path, path);
			strcpy(copy_path, link_path);
			path = copy_path;
		}
#endif							// S_IFLNK
*/
		*new_path++ = '/';
	}
	/* Delete trailing slash but don't whomp a lone slash. */
	if (new_path != got_path + 1 && new_path[-1] == '/')
		new_path--;
	/* Make sure it's null terminated. */
	*new_path = '\0';
	strcpy(resolved_path, got_path);
	return resolved_path;
}
//#endif

void free_dir(dir_t * const dir)
{
    if (dir != NULL)
    {
        if (dir->path != NULL)
        {
            free(dir->path);
        }

        if (dir->entries != NULL)
        {
            for (int32_t i = 0; i < dir->entry_count; i++)
            {
                if (dir->entries[i].name != NULL)
                {
                    free(dir->entries[i].name);
                }
            }
            free(dir->entries);
        }
        free(dir);
    }
}

dir_t *get_dir(const char * const dir_path) 
{
    struct dirent *ep;

    // Would normally just use dp and not ndp,
    // but for some reason on 3DS and Switch 
    // rewindir appears to do absolutely nothing
    DIR *dp = opendir(dir_path);
    DIR *ndp = opendir(dir_path);

    if (dp == NULL || ndp == NULL)
    {
        if (dp != NULL)
        {
            closedir(dp);
        }

        if (ndp != NULL)
        {
            closedir(ndp);
        }

        return NULL;
    }
   
    dir_t *dir = calloc(1, sizeof(dir_t));
    if (dir == NULL)
    {
        goto err;
    }
   
    char *path = calloc(1, PATH_MAX);
    char *result = realpath_switch(dir_path, path);

    if (result == NULL)
    {
        goto err;
    }

	// Each Path is probably nowhere near PATH_MAX in size
	char *minimised_path = realloc(path, strlen(path) + 1);

    dir->path = minimised_path;
    
    // Loop through twice, once to get the number of files
    // so we know how many slots to allocate, then a second time
    // to allocate the filenames to the slots
    uint32_t entry_count = 0;
    while ((ep = readdir(dp)))
    {
        entry_count++;
    }
    
    // Allocate slots for number of entires
    
    #ifdef __SWITCH__
        entry_count += 2; // For some reason Switch's API doesn't include "." and ".."
    #endif

    dir->entries = calloc(entry_count, sizeof(dir_entry_t));
    if (dir->entries == NULL)
    {
        goto err;
    }

    uint32_t entries_read = 0; 
    
    #ifdef __SWITCH__
        dir->entries[0].name = calloc(1, strlen(".") + 1);
        if (dir->entries[0].name == NULL)
        {
            goto err;
        }
        strcpy(dir->entries[0].name, ".");
        dir->entries[0].is_dir = 1;
        entries_read++;

        dir->entries[1].name = calloc(1, strlen("..") + 1);
        if (dir->entries[1].name == NULL)
        {
            goto err;
        }
        strcpy(dir->entries[1].name, "..");
        dir->entries[1].is_dir = 1;
        entries_read++;

    #endif
    
    
    uint32_t i = 0;
    #ifdef __SWITCH__
        i += 2;
    #endif

    for (; (ep = readdir(ndp)) && i < entry_count; i++)
    {
       dir->entries[i].name = calloc(1, strlen(ep->d_name)+1);
       if (dir->entries[i].name == NULL)
       {
           goto err;
       }
       strcpy(dir->entries[i].name, ep->d_name);
       dir->entries[i].is_dir = ep->d_type == DT_DIR;
       entries_read++; 
    }

    dir->entry_count = entries_read;

    closedir(dp);
    closedir(ndp);
    return dir;

    err:
        free_dir(dir);
        closedir(dp);
        closedir(ndp);
        return NULL;
}
