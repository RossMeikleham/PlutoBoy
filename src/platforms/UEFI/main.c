/** @file
	UEFI Plutoboy entry point
**/

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/UefiRuntimeLib.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/EfiShellParameters.h>

#include "../../non_core/logger.h"
#include "../../core/mmu/memory.h"
#include "../../core/emu.h"

#include "../../core/serial_io.h"

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PlutoboyMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  	EFI_SHELL_PARAMETERS_PROTOCOL *EfiShellParametersProtocol;
  	EFI_SHELL_INTERFACE           *EfiShellInterface;
  	EFI_STATUS                    Status;

	CHAR16 **argv = NULL;
	INTN argc = 0;

	EfiShellParametersProtocol = NULL;
  	EfiShellInterface = NULL;

  	Status = SystemTable->BootServices->OpenProtocol(ImageHandle,
                             	&gEfiShellParametersProtocolGuid,
                             	(VOID **)&EfiShellParametersProtocol,
                             	ImageHandle,
                             	NULL,
                             	EFI_OPEN_PROTOCOL_GET_PROTOCOL
                            	);
  	if (!EFI_ERROR(Status)) {
    	//
    	// use shell 2.0 interface
    	//
		argc = EfiShellParametersProtocol->Argc;
		argv = EfiShellParametersProtocol->Argv;
                      
  	} else {
    	//
    	// try to get shell 1.0 interface instead.
    	//
    	Status = SystemTable->BootServices->OpenProtocol(ImageHandle,
                               	&gEfiShellInterfaceGuid,
                               	(VOID **)&EfiShellInterface,
                               	ImageHandle,
                               	NULL,
                               	EFI_OPEN_PROTOCOL_GET_PROTOCOL
                              	);
    	if (!EFI_ERROR(Status)) {
      		//
      		// use shell 1.0 interface
      		//
      		argc = EfiShellInterface->Argc;
      		argv = EfiShellInterface->Argv;
                  
    	} else {
      		ASSERT(FALSE);
    	}
	}

	if (argc < 2) {
		Print(L"Usage: plutoboy.efi rom\n");
		return 1;
	}

	char *file_name = malloc(StrLen(argv[1]) + 1);
	if (file_name == NULL) {
		return 1;
	}
	
	for (int i = 0; i < StrLen(argv[1]) * 2; i++) {
		file_name[i] = (char)(argv[1][i]);
	}

	file_name[StrLen(argv[1])] = '\0';
 
    int debug = 0;
    int dmg_mode = 0;
		
    set_log_level(LOG_INFO);
	log_message(LOG_INFO, "Test log %d\n", 27);
	log_message(LOG_ERROR, "Test log hex 0x%x\n", 0x10);
	log_message(LOG_WARN, "Test log HEX 0x%X\n", 0x10);
	log_message(LOG_INFO, "About to open file %s\n", "rom.gb");

    ClientOrServer cs = NO_CONNECT;
    
    if (!init_emu(file_name, debug, dmg_mode, cs)) {
        Print(L"Failed to init emulator\n");
		log_message(LOG_ERROR, "failed to load file\n");
        free(file_name);
		return 1;
    }
    log_message(LOG_INFO, "exiting\n");
	
    run();
    free(file_name);
	return EFI_SUCCESS;
}
