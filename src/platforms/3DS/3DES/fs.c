// fs.c : For all filesystem related functions
#include "common.h"
#include "fs.h"
#include "err.h"

// Sort filename array (yay for bubble sort)
void fs_bubble_sort_filearr_(void) {
  bool swapped = true;
  file_entry temp;
  int pass, pass2;
  for (pass=size_of_file_array-1; pass>0; pass--) {
    for (pass2=0; pass2<pass; pass2++) {
      if (strcasecmp(file_arr[pass2].name, file_arr[pass2+1].name) > 0) {
        swapped = true;
        // Swap values in file name array
        temp = file_arr[pass2];
        file_arr[pass2] = file_arr[pass2+1];
        file_arr[pass2+1] = temp;
      }
    }
    if (!swapped) { break; }
  }
}

// Sets current_path to its upper directory
void fs_get_ud(void) {
  consoleSelect(&debug_screen);
  char path_to_iterate[MAX_PATH_SIZE];
  char looking_for[] = "/";
  char *token;
  char dummy1[MAX_PATH_SIZE] = "";
  char dummy2[MAX_PATH_SIZE] = "";

  strncpy(path_to_iterate, current_path, MAX_PATH_SIZE);
  token = strtok(path_to_iterate, looking_for);
  while (token != NULL) {
    strcat(dummy1, token);
    strcat(dummy1, "/");
    // If dummy1 has been fully constructed into current_path
    if (!strcmp(dummy1, current_path)) {
      // dummy2 happens after this, so will have 1 less token
      strncpy(current_path, dummy2, MAX_PATH_SIZE);
      break;
    }
    strcat(dummy2, token);
    strcat(dummy2, "/");
    // get the next token
    token = strtok(NULL, looking_for);
  }
  printf("%snew path: %s%s\n", FG_GREEN, current_path, RESET);
}

// Fills file array with all files in a given directory
void fs_populate_filarr(char dir_to_get[]) {
  consoleSelect(&debug_screen);
  // 2 of each for 2 iterations
  DIR *d;
  DIR *nd;
  d = opendir(dir_to_get);
  nd = opendir(dir_to_get);

  if (d) {
    struct dirent *dir;
    struct dirent *ndir;
    int count = 0;
    selected  = 0;
    scroll    = 0;
    // While readdir returns something other than NULL. The variable dir will change each loop
    while ((dir = readdir(d)) != NULL) {
      // Count how many files there are
      count++;
    }
    size_of_file_array = count;
    count = 0;
    file_arr = realloc(file_arr, (size_of_file_array+1) * sizeof(file_entry));
    if (file_arr == NULL) {
      err_show("fs.c", __LINE__-2, "file_arr memory allocation failed");
    }

    else {
      // Iterate over dir again, this time adding filenames to created 2D array
      while ((ndir = readdir(nd)) != NULL) {
        // Get d_name from the dir struct and copy into array
        strncpy(file_arr[count].name, ndir->d_name, MAX_DIR_NAME_SIZE);
        // If d_type is a file
        file_arr[count].isfile = (ndir->d_type == 8);
        count++;
      }
      fs_bubble_sort_filearr_();
    }
  }
  closedir(d);
  closedir(nd);
}

void fs_delete_dir_recursivley_(char path_to_delete[MAX_PATH_SIZE]) {
  consoleSelect(&debug_screen);
  DIR *d;
  d = opendir(path_to_delete);
  int ret;

  if (d) {
    struct dirent *dir;
    char new_path_to_delete[MAX_PATH_SIZE];

    while ((dir = readdir(d)) != NULL) {
      strncpy(new_path_to_delete, path_to_delete, MAX_PATH_SIZE);
      strcat(new_path_to_delete, "/");
      strcat(new_path_to_delete, dir->d_name);
      if (dir->d_type == 8) {
        ret = remove(new_path_to_delete);
        if (ret) {
          printf("%sError: unable to delete file %s%s\n", BG_RED, dir->d_name, RESET);
        }
        else {
          printf("%sDeleted file: %s%s\n", FG_MAGENTA, dir->d_name, RESET);
        }
      }
      else {
        fs_delete_dir_recursivley_(new_path_to_delete);
      }
    }
    closedir(d);
    ret = rmdir(path_to_delete);
    if (ret) {
      printf("%sError: unable to delete dir%s\n", BG_RED, RESET);
    }
    else {
      printf("%sDeleted dir %s%s\n", FG_MAGENTA, path_to_delete, RESET);
    }
  }
  else {
    // Dir doesen't exist?
    closedir(d);
  }
}

void fs_delete_selected(void) {
  consoleSelect(&debug_screen);
  char filepath[MAX_PATH_SIZE];
  strncpy(filepath, current_path, MAX_PATH_SIZE);
  strcat(filepath, file_arr[selected+scroll].name);
  int ret;

  // If it is a dir
  if (!file_arr[selected+scroll].isfile) {
    ret = rmdir(filepath);
    if (!ret) {
      printf("%s%s deleted%s\n", FG_MAGENTA, file_arr[selected+scroll].name, RESET);
    }
    else {
      fs_delete_dir_recursivley_(filepath);
    }
  }
  else {
    ret = remove(filepath);
    if (!ret) {
      printf("%s%s deleted%s\n", FG_MAGENTA, file_arr[selected+scroll].name, RESET);
    }
    else {
      printf("%sUnable to delete %s%s\n", BG_RED, file_arr[selected+scroll].name, RESET);
    }
  }
}
