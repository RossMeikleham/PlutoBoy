#include <switch.h>
#include "common.h"
#include "fs.h"
#include "draw.h"
#include "btn.h"
#include "ctm.h"
#include "err.h"

char current_path[511];
int selected = 0;
int scroll   = 0;

file_entry *file_arr;
int size_of_file_array;

PrintConsole top_screen, instruction_screen;//, debug_screen;

CB clipboard;
bool quit_for_err = false;

int selectFile(int *cgb, char *file_name) {

  int selected_file = 0;
 
  nifmInitialize();  
  //gfxInitDefault();
  consoleInit(&top_screen);
  consoleSetWindow(&top_screen, 0, 11, 80, 45);
  consoleInit(&instruction_screen);
  consoleSetWindow(&instruction_screen, 0, 0, 80, 10);
  consoleSelect(&instruction_screen);

  /*
  consoleInit(GFX_BOTTOM, &debug_screen);
  consoleInit(GFX_BOTTOM, &instruction_screen);
  // x, y, width, height
  consoleSetWindow(&instruction_screen, 0, 0, 40, 9);
  consoleSetWindow(&debug_screen, 0, 10, 40, 20);
*/
  printf("Select ROM to run\n");
  printf("\n----------------------------------------\n");
  printf("A - cd / open rom in CGB mode\nY - cd / open rom in DMG mode\nB - go up a directory\nX - cd to /\n");
  printf("----------------------------------------\n");

//  consoleSelect(&debug_screen);

  // Initial allocation
  file_arr = malloc(1 * sizeof(file_entry));
  if (file_arr == NULL) {
    err_show("3des.c", __LINE__-2, "file_arr memory allocation failed");
  }

  // For when it is first realloc(ed)
  size_of_file_array = 1;
  // Copy root dir into current_path
  strcpy(current_path, "sdmc:/");
   
  // Init context menu
  ctm_init(); 
  // Fill file name array with file names
  fs_populate_filarr(current_path); 
 
  // Print all in root dir  
  draw_filearr(1);
   
  
  // Main loop
  while (appletMainLoop()) {
    //gfxWaitForVsync();
    hidScanInput();

    if (quit_for_err) {
      break;
    }
    u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    
    if (kDown & KEY_PLUS) {
      break;
    }

    else if (kDown & KEY_UP) {
      btn_up();
      draw_filearr(0);
    }

    else if (kDown & KEY_DOWN) {
      btn_down();
      draw_filearr(0);
    }

    else if (kDown & KEY_LEFT) {
      btn_left();
      draw_filearr(0);
    }

    else if (kDown & KEY_RIGHT) {
      btn_right();
      draw_filearr(0);
    }

    else if (kDown & KEY_X) {
      //consoleSelect(&debug_screen);
      //printf("%sKEY_X pressed%s\n", FG_BLUE, RESET);
      strcpy(current_path, "sdmc:/");
      printf("%snew path: %s%s\n", FG_GREEN, current_path, RESET);
      fs_populate_filarr(current_path);
      draw_filearr(1);
    }

    else if (kDown & KEY_A) { 
        if (btn_a_pressed(cgb) == 1) {
            strncpy(file_name, current_path, 1024);
            selected_file = 1;
            break;
        }; 
    }

    
    else if (kDown & KEY_B) { btn_b_pressed(); }

    else if (kDown & KEY_Y) { 
        if (btn_y_pressed(cgb) == 1) {
            strncpy(file_name, current_path, 1024);
            selected_file = 1;
            break;
        }; 
    }

    consoleUpdate(NULL);
    // Flush and swap framebuffers
    //gfxFlushBuffers();
    //gfxSwapBuffers();
  }

  free(file_arr);
  return selected_file;
}
