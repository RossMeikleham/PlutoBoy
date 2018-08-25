#include "common.h"
#include "btn.h"
#include "fs.h"
#include "draw.h"
#include "ctm.h"

void btn_up(void) {
  if (size_of_file_array != 0) {
    if (selected+scroll == 0) {
      if (size_of_file_array > MAX_FILES_ON_SCREEN) {
        selected = MAX_FILES_ON_SCREEN-1;
        // scroll will be max size it can be
        scroll = size_of_file_array-MAX_FILES_ON_SCREEN;
      }
      else {
        selected = size_of_file_array-1;
      }
    }

    else if (scroll > 0) {
      scroll--;
    }

    else {
      selected--;
    }
  }
}

void btn_down(void) {
  if (size_of_file_array != 0) {
    if (selected+scroll == size_of_file_array-1) {
      selected = 0;
      scroll = 0;
    }

    else if ((selected == MAX_FILES_ON_SCREEN-1) && (selected+scroll < size_of_file_array-1)) {
      scroll++;
    }

    else {
      selected++;
    }
  }
}

void btn_left(void) {
  if (size_of_file_array != 0) {
    selected = 0;
    scroll = 0;
  }
}

void btn_right(void) {
  if (size_of_file_array != 0) {
    if (size_of_file_array > MAX_FILES_ON_SCREEN) {
      selected = MAX_FILES_ON_SCREEN-1;
      scroll = size_of_file_array-MAX_FILES_ON_SCREEN;
    }
    else {
      selected = size_of_file_array-1;
    }
  }
}

int btn_a_pressed(int *cgb_mode) {
  if (size_of_file_array != 0) {
    // If it is actually a directory
    if (!file_arr[selected+scroll].isfile) {
      strcat(current_path, file_arr[selected+scroll].name);
      strcat(current_path, "/");

      consoleSelect(&debug_screen);
      printf("%snew path: %s%s\n", FG_GREEN, current_path, RESET);

      fs_populate_filarr(current_path);
      draw_filearr(1);
    }

    else {
      // opening ROM
      /*consoleSelect(&debug_screen);
      printf("%sOpening file context menu%s\n", FG_MAGENTA, RESET);
      ctm_open();
      fs_populate_filarr(current_path);
      draw_filearr(1);*/
      strcat(current_path, file_arr[selected+scroll].name);
      *cgb_mode = 1;
      return 1;
    }
  }
  return 0;
}

void btn_b_pressed(void) {
  if (!strcmp(current_path, "sdmc:/")) {
    consoleSelect(&debug_screen);
    printf("%scurrently in sdmc:/%s\n", FG_GREEN, RESET);
  }
  else {
    // move up a directory
    fs_get_ud();
    fs_populate_filarr(current_path);
    draw_filearr(1);
  }
}

int btn_y_pressed(int *cgb_mode) {
  if (size_of_file_array != 0) {
    // If it is actually a directory
    if (!file_arr[selected+scroll].isfile) {
      strcat(current_path, file_arr[selected+scroll].name);
      strcat(current_path, "/");

      consoleSelect(&debug_screen);
      printf("%snew path: %s%s\n", FG_GREEN, current_path, RESET);

      fs_populate_filarr(current_path);
      draw_filearr(1);
    }

    else {
      // opening ROM
      /*consoleSelect(&debug_screen);
      printf("%sOpening file context menu%s\n", FG_MAGENTA, RESET);
      ctm_open();
      fs_populate_filarr(current_path);
      draw_filearr(1);*/
      strcat(current_path, file_arr[selected+scroll].name);
      *cgb_mode = 0;
      return 1;
    }
  }
  return 0;
}
