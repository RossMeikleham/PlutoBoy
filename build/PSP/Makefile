MAIN_DIR=../../src/platforms/PSP
COMMON_DIR = $(MAIN_DIR)/common
CORE_DIR=../../src/core
MMU_DIR=$(CORE_DIR)/mmu
IO_DIR=../../src/shared_libs/PSP
FILE_BROWSER_DIR=$(MAIN_DIR)/file_browser

MAIN_DIR_FILES = $(wildcard $(MAIN_DIR)/*.c)
COMMON_DIR_FILES = $(wildcard $(COMMON_DIR)/*.c)
CORE_DIR_FILES = $(wildcard $(CORE_DIR)/*.c)
MMU_DIR_FILES = $(wildcard $(MMU_DIR)/*.c)
IO_DIR_FILES = $(wildcard $(IO_DIR)/*.c)
FILE_BROWSER_FILES =$(wildcard $(FILE_BROWSER_DIR)/*.c)

C_FILES = $(MAIN_DIR_FILES) $(COMMON_DIR_FILES) $(CORE_DIR_FILES) $(MMU_DIR_FILES) $(IO_DIR_FILES) $(FILE_BROWSER_FILES)
OBJ_FILES = $(addsuffix .o, $(basename $(C_FILES)))


TARGET		= gameboy
OBJS		= $(OBJ_FILES)

#Compile for PSP Kernel 6.60
PSP_FW_VERSION = 660

#callback.o:
#	$(CC) $(CFLAGS) $(MAIN_DIR)/common/callback.c
 
INCDIR		=
PSPBIN = $(PSPSDK)/../bin
CFLAGS		= -std=c99 -G4 -Wall -O2
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)
OUTDIR = Gameboy


LIBDIR		=
LDFLAGS	=
LIBS		= -lm
 
CFLAGS += `$(PSPBIN)/sdl-config --cflags`
LIBS += `$(PSPBIN)/sdl-config --libs`

BUILD_PRX = 1 
 
EXTRA_TARGETS	= EBOOT.PBP
PSP_EBOOT_TITLE= Gameboy
 
PSPSDK	= $(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak


directory:
	mkdir -p  $(OUTDIR)
	mkdir -p $(OUTDIR)/games
	cp -r "Resources" $(OUTDIR)
	cp "EBOOT.PBP" $(OUTDIR)
