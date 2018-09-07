#include "common.h"
#include "ctm.h"
#include "sha256.h"
#include "draw.h"

#define ctm_items_len 3
#define ctm_item_str_len 20

char ctm_items[ctm_items_len][ctm_item_str_len];
int ctm_selected;

void ctm_init(void) {
  strncpy(ctm_items[0], "Get SHA256 Hash", ctm_item_str_len);
  strncpy(ctm_items[1], "Cut", ctm_item_str_len);
  strncpy(ctm_items[2], "Copy", ctm_item_str_len);
}

void ctm_draw_menu_(int clr) {
  if (clr) {
    draw_clearscrn();
  }
  consoleSelect(&top_screen);
  printf(RESET_TO_TOP_LEFT);
  printf("%s%s%s\n\n", BLACK_ON_WHITE, file_arr[selected+scroll].name, RESET);
  for (int i=0; i<ctm_items_len; i++) {
    if (i == ctm_selected) {
      printf("\t-> %s\n", ctm_items[i]);
    }
    else {
      printf("\t   %s\n", ctm_items[i]);
    }
  }
  printf("\nPress B to go back\n");
}

void ctm_open(void) {
  ctm_selected = 0;

  ctm_draw_menu_(1);
  while (aptMainLoop()) {
    gspWaitForVBlank();
    hidScanInput();
    u32 ctm_kDown = hidKeysDown();

    if (ctm_kDown & KEY_UP) {
      if (!ctm_selected) {
        ctm_selected = ctm_items_len-1;
      }
      else {
        ctm_selected--;
      }
      ctm_draw_menu_(0);
    }

    else if (ctm_kDown & KEY_DOWN) {
      if (ctm_selected == ctm_items_len-1) {
        ctm_selected = 0;
      }
      else {
        ctm_selected++;
      }
      ctm_draw_menu_(0);
    }

    else if (ctm_kDown & KEY_A) {
      switch (ctm_selected) {
        case 0:
          sha256_current_file();
          ctm_draw_menu_(1);
          break;
        case 1:
          break;
        case 2:
          break;
      }
    }

    else if (ctm_kDown & KEY_B) {
      draw_clearscrn();
      break;
    }

    gfxFlushBuffers();
    gfxSwapBuffers();
  }
}
