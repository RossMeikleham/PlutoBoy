#include "../../non_core/graphics_out.h"

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/UefiRuntimeLib.h>

static EFI_GRAPHICS_OUTPUT_BLT_PIXEL *pixels;
static EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput = NULL;
static int screen_width;
static int screen_height;

/* Initialize graphics
 * returns 1 if successful, 0 otherwise */
int init_screen(int win_x, int win_y, uint32_t *p) {
 
	UINTN HandleCount;
	EFI_HANDLE  *HandleBuffer = NULL;
    
    screen_width = win_x;
    screen_height = win_y;

    pixels = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)p;

   // Locate all instances of GOP
   EFI_STATUS Status = gBS->LocateHandleBuffer(ByProtocol
	, &gEfiGraphicsOutputProtocolGuid, NULL, &HandleCount, &HandleBuffer);

   if (EFI_ERROR (Status)) {
		Print(L"ShowStatus: Graphics output protocol not found\n");
		return 0;
	}
  	
    Print(L"Graphics output protocols found!\n");
	
	for (UINTN i = 0; i < HandleCount; i++) {
		Status = gBS->HandleProtocol(HandleBuffer[i]
			, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
        
		if (EFI_ERROR (Status)) {
            Print(L"ShowStatus: gBS->HandleProtocol[%d] returned %r\n", i, Status);
            continue;
        } else {
            return 1;
        }

    }
	    
    return 0; // Failed
}

void draw_screen() {
		GraphicsOutput->Blt(GraphicsOutput, pixels, EfiBltBufferToVideo, 0, 0, 0, 0, 160, 144, 0);

}
