#include "common.h"
#include "draw.h"

// Print all strings in the file name array and set highlighted line
void draw_filearr(int clr) {
  if (clr) {
    draw_clearscrn();
  }
  int max_files_to_print;
  if (size_of_file_array < MAX_FILES_ON_SCREEN) {
    max_files_to_print = size_of_file_array;
  } else {
    max_files_to_print = MAX_FILES_ON_SCREEN;
  }
  consoleSelect(&top_screen);
  // Moves the cursor to the top left corner of the screen
  printf(RESET_TO_TOP_LEFT);

  if (strlen(current_path) <= 50) {
    printf("%s%s%s\n", BLACK_ON_WHITE, current_path, RESET);
  } else {
    printf("%s%s%s", BLACK_ON_WHITE, current_path+strlen(current_path)-50, RESET);
  }

  if (max_files_to_print > 0) {
    if (scroll > 0) {
      printf("/\\\n");
    } else {
      // Keep all the files in the same place on screen, erase an arrow if there was one
      printf("  \n");
    }
    for (int i=0; i<max_files_to_print; i++) {
      if (!file_arr[i].isfile) {
        if (i == selected) {
          printf("\n D | %s%-39.39s%s", BLACK_ON_WHITE, file_arr[i+scroll].name, RESET);
        } else {
          printf("\n D | %-39.39s", file_arr[i+scroll].name);
        }
      } else {
        if (i == selected) {
          printf("\n F | %s%-39.39s%s", BLACK_ON_WHITE, file_arr[i+scroll].name, RESET);
        } else {
          printf("\n F | %-39.39s", file_arr[i+scroll].name);
        }
      }
    }

    if ( (size_of_file_array > MAX_FILES_ON_SCREEN) && (selected+scroll != size_of_file_array-1) ) {
      printf("\n\n\\/");
    } else {
      printf("\n\n  ");
    }
  } else {
    printf("\n\n\t\t%s- Folder is empty -%s", BLACK_ON_WHITE, RESET);
  }
}

// Clear the screen
void draw_clearscrn(void) {
  consoleSelect(&top_screen);
  printf(RESET_TO_TOP_LEFT);
  // Fill the screen with blank spaces
  for (int i=0; i<37; i++) { printf("%-40.40s", " "); }
  printf(RESET_TO_TOP_LEFT);
}

int draw_delete_dialouge(void) {
  draw_clearscrn();
  // Top screen already selected form draw_clearscrn
  printf("\n\n\n\t\t%sDelete %-35.35s%s", FG_RED, file_arr[selected+scroll].name, RESET);
  printf("\n\n\t\t[A] - Yes\n\t\t[B] - No");

  while (aptMainLoop()) {
    gspWaitForVBlank();
    hidScanInput();
    u32 exitkDown = hidKeysDown();
    if (exitkDown & KEY_A) {
      return 0;
    }
    else if (exitkDown & KEY_B) {
      return 1;
    }
    gfxFlushBuffers();
    gfxSwapBuffers();
  }
  // If something goes wrong (+ stops compile warning)
  return 1;
}
