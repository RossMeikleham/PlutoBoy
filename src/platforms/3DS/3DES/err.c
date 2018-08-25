#include "common.h"

void err_dump_to_file_(char message[]) {
  // Dump message, time & common variable values & pointers to file
}

void err_show(char file_name[], int line, char message[]) {
  quit_for_err = true;
  // Reset all screens
  consoleInit(GFX_TOP, &top_screen);
  consoleInit(GFX_BOTTOM, &debug_screen);
  consoleInit(GFX_BOTTOM, &instruction_screen);
  consoleSelect(&top_screen);
  printf("%s\n\t\tFATAL ERROR\n\n%s", BG_RED, RESET);
  printf("%s\n", file_name);
  printf("line %d\n", line);
  printf(message);
  printf("\n\nPlease raise an issue at:\ngithub.com/thatguywiththatname/3DES/issues\n");
  printf("With details of this error &\nwhat you were doing at the time\n");
  printf("\nPress A to close app\n");
  while (aptMainLoop()) {
    gspWaitForVBlank();
    hidScanInput();
    u32 exitkDown = hidKeysDown();
    if (exitkDown & KEY_A) {
      return;
    }
    gfxFlushBuffers();
    gfxSwapBuffers();
  }
}
