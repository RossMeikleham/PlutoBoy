MAIN_DIR=../../src/platforms/PSP
COMMON_DIR = $(MAIN_DIR)/common
CORE_DIR=../../src/core
MMU_DIR=$(CORE_DIR)/mmu
IO_DIR=../../src/shared_libs/PSP

MAIN_DIR_FILES = $(wildcard $(MAIN_DIR)/*.c)
COMMON_DIR_FILES = $(wildcard $(COMMON_DIR)/*.c)
CORE_DIR_FILES = $(wildcard $(CORE_DIR)/*.c)
MMU_DIR_FILES = $(wildcard $(MMU_DIR)/*.c)
IO_DIR_FILES = $(wildcard $(IO_DIR)/*.c)

C_FILES = $(MAIN_DIR_FILES) $(COMMON_DIR_FILES) $(CORE_DIR_FILES) $(MMU_DIR_FILES) $(IO_DIR_FILES)
OBJ_FILES = $(addsuffix .o, $(basename $(C_FILES)))


TARGET		= gameboy
OBJS		= $(OBJ_FILES)

#callback.o:
#	$(CC) $(CFLAGS) $(MAIN_DIR)/common/callback.c
 
INCDIR		=
CFLAGS		= -std=c99 -G4 -Wall -O2
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)
 
LIBDIR		=
LDFLAGS	=
LIBS		= -lm
 
BUILD_PRX = 1 
 
EXTRA_TARGETS	= EBOOT.PBP
PSP_EBOOT_TITLE= Gameboy
 
PSPSDK	= $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
