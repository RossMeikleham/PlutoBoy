MAIN_DIR=../../src/platforms/PSP
COMMON_DIR = $(MAIN_DIR)/common
CORE_DIR=../../src/core
MMU_DIR=$(CORE_DIR)/mmu
AUDIO_DIR=$(CORE_DIR)/audio
IO_DIR=../../src/shared_libs/SDL
FILE_BROWSER_DIR=$(MAIN_DIR)/file_browser
SOUND_DIR=../../src/shared_libs/SDL/audio

MAIN_DIR_FILES = $(wildcard $(MAIN_DIR)/*.c)
COMMON_DIR_FILES = $(wildcard $(COMMON_DIR)/*.c)
CORE_DIR_FILES = $(wildcard $(CORE_DIR)/*.c)
MMU_DIR_FILES = $(wildcard $(MMU_DIR)/*.c)
AUDIO_FILES = $(wildcard $(AUDIO_DIR)/*.cpp)
IO_DIR_FILES = $(wildcard $(IO_DIR)/*.c)
IO_DIR_FILES2 = $(wildcard $(IO_DIR)/*.cpp)
TIME_FILES = $(wildcard $(IO_DIR)/../*.c)
FILE_BROWSER_FILES =$(wildcard $(FILE_BROWSER_DIR)/*.c)
SOUND_FILES = $(wildcard $(SOUND_DIR)/*.cpp)

C_FILES = $(MAIN_DIR_FILES) $(COMMON_DIR_FILES) $(CORE_DIR_FILES) $(MMU_DIR_FILES) $(IO_DIR_FILES) $(FILE_BROWSER_FILES) $(IO_DIR_FILES2) $(SOUND_FILES) $(AUDIO_FILES) $(TIME_FILES)
OBJ_FILES = $(addsuffix .o, $(basename $(C_FILES)))


TARGET		= gameboy
OBJS		= $(OBJ_FILES)

BUILD_PRX = 1
#Compile for PSP Kernel 6.60
PSP_FW_VERSION = 150
#Give stack size of 1MB
PSP_MAINTHREAD_STACK_SIZE_KB = 1024
#callback.o:
#	$(CC) $(CFLAGS) $(MAIN_DIR)/common/callback.c
 
INCDIR		=
PSPBIN = $(PSPSDK)/../bin
CFLAGS		= -std=c99 -G0 -Wall -O3
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)
OUTDIR = Plutoboy

USE_KERNEL_LIBC = 1 
USE_KERNEL_LIBS = 1 

LIBDIR		=
LDFLAGS	=
LIBS		= -lstdc++ -lm
 
CFLAGS += `$(PSPBIN)/sdl-config --cflags`
LIBS += `$(PSPBIN)/sdl-config --libs`

EXTRA_TARGETS	= EBOOT.PBP
PSP_EBOOT_TITLE= Gameboy
 
PSPSDK	= $(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak

directory:
	mkdir -p  $(OUTDIR)
	mkdir -p $(OUTDIR)/games
	cp -r "Resources" $(OUTDIR)
	cp "EBOOT.PBP" $(OUTDIR)
