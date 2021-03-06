#include "../../../non_core/filebrowser.h"
#include "../../../non_core/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// "Desktop" i.e. Windows/OSX/Linux

//#ifdef POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
//#endif

#ifdef PSVITA
#define PATH_MAX 4096
#endif


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
            for (uint32_t i = 0; i < dir->entry_count; i++)
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


// VitaSDK lacks the API to do realpath, getcwd, etc.
#if defined(PSVITA) || defined(__SWITCH__)
    // Our reduced path will always be smaller than the full path
   char *path = strdup(dir_path);
    if (path == NULL)
    {
        return NULL;
    }

    // "Simplify" the Directory path if it ends with "." or ".."
	char *last_dir_chr = strrchr(path, '/');
	if (last_dir_chr != NULL) 
    {
        char *fname = last_dir_chr + 1;
        if (last_dir_chr != path)
        {
            // Same Dir
            if (!strcmp(fname, ".")) 
            {
                // x/y/. -> x/y
                // /x/. -> /x
                if (last_dir_chr != path) 
                {
                    last_dir_chr[0] = '\0';
                }
                // /. -> /
                else 
                {
                    last_dir_chr[1] = '\0';
                }
            }
            // Parent Dir
            else if (!strcmp(fname, "..")) 
            {
                // /.. -> /
                if (last_dir_chr == path) 
                {
                    last_dir_chr[1] = '\0';
                }
                // /x/y/.. -> /x/y
                // x/y/..  ->  x/y
                // /x/.. -> /x
                // x/.. -> x
                else
                {
                    last_dir_chr[0] = '\0';
                }

            
                // Finally
                // /x/y -> /x   (/x/y/.. -> /x)
                // x/y -> x     (x/y/.. -> x)
                // /x -> /x     (/x/.. -> /x)
                // x -> x       (x/.. -> x)
                char *parent_dir_chr = strrchr(path, '/');
                if (parent_dir_chr != NULL && parent_dir_chr != path) 
                {
                    parent_dir_chr[0] = '\0';
                }
            }
	    }
    }
#else
    log_message(LOG_INFO, "Calling realpath: %s\n", dir_path);
    char *path = realpath(dir_path, NULL);
    if (path == NULL)
    {
        return NULL;
    }
#endif
    log_message(LOG_INFO, "%s dir_path, reduced to %s\n", dir_path, path);
    //path = strdup(dir_path);
    
    // Would normally just use dp and not ndp,
    // but for some reason on 3DS and Switch 
    // rewindir appears to do absolutely nothing
    DIR *dp = opendir(path);
    DIR *ndp = opendir(path);
    dir_t *dir = NULL;

    if (dp == NULL || ndp == NULL)
    {
        goto err;
    }
   
    dir = calloc(1, sizeof(dir_t));
    if (dir == NULL)
    {
        goto err;
    }
   
    //char *path = calloc(1, PATH_MAX);
    //todo path = 
    //char *result = realpath_switch(dir_path, path);

    dir->path = path;
    
    // Loop through twice, once to get the number of files
    // so we know how many slots to allocate, then a second time
    // to allocate the filenames to the slots
    uint32_t entry_count = 0;
    while ((ep = readdir(dp)))
    {
        entry_count++;
    }
    
    // Allocate slots for number of entires
    
    #if defined(__SWITCH__) || defined(PSVITA)
        entry_count += 2; // For some reason Switch's API doesn't include "." and ".."
    #endif
    log_message(LOG_INFO, "entry count %u\n", entry_count);

    dir->entries = calloc(entry_count, sizeof(dir_entry_t));
    if (dir->entries == NULL)
    {
        goto err;
    }

    uint32_t entries_read = 0; 
    
    #if defined(__SWITCH__) || defined(PSVITA)
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
    #if defined(__SWITCH__) || defined(PSVITA)
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
       
       char *file_path = calloc(strlen(dir->path) + 1 + strlen(ep->d_name) + 1, 1);
       if (file_path == NULL) 
       {
           goto err;
       } 
       strcpy(file_path, dir->path);
       file_path[strlen(file_path)] = '/';
       strcpy(file_path + strlen(dir->path) + 1, ep->d_name);
       log_message(LOG_INFO, "FILE PATH: %s\n", file_path);
       struct stat path_stat;
       stat(file_path, &path_stat);
       free(file_path);
       dir->entries[i].is_dir = S_ISDIR(path_stat.st_mode);

       //dir->entries[i].is_dir = ep->d_type == DT_DIR;
       entries_read++; 
    }

    dir->entry_count = entries_read;

    closedir(dp);
    closedir(ndp);
    return dir;

    err:
        if (dir != NULL)
        {
            free_dir(dir);
        }
        else
        {
            free(path);
        }

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
