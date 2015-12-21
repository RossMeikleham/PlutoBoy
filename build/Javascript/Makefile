MAIN_DIR=../../src/platforms/standard
MAIN_FILE=../../src/platforms/Javascript/main.c
COMMON_DIR = $(MAIN_DIR)/common
CORE_DIR=../../src/core
MMU_DIR=$(CORE_DIR)/mmu
AUDIO_DIR=$(CORE_DIR)/audio
IO_DIR=../../src/shared_libs/SDL2
IO_DIR2=../../src/shared_libs
SOUND_DIR=../../src/shared_libs/SDL2/audio
FILE_BROWSER_DIR=$(MAIN_DIR)/file_browser

MAIN_DIR_FILES = $(filter-out $(MAIN_DIR)/main.c, $(wildcard $(MAIN_DIR)/*.c)) 

COMMON_DIR_FILES = $(wildcard $(COMMON_DIR)/*.c)
CORE_DIR_FILES = $(wildcard $(CORE_DIR)/*.c)
MMU_DIR_FILES = $(wildcard $(MMU_DIR)/*.c)
AUDIO_FILES = $(wildcard $(AUDIO_DIR)/*.cpp)
IO_DIR_FILES = $(wildcard $(IO_DIR)/*.c)
IO_DIR_FILES2 = $(wildcard $(IO_DIR)/*.cpp)
IO_DIR_FILES3 = $(wildcard $(IO_DIR2)/*.c)
SOUND_FILES = $(wildcard $(SOUND_DIR)/*.cpp)
FILE_BROWSER_FILES =$(wildcard $(FILE_BROWSER_DIR)/*.c)


C_FILES = $(MAIN_DIR_FILES) $(COMMON_DIR_FILES) $(CORE_DIR_FILES) $(MMU_DIR_FILES) $(IO_DIR_FILES) $(FILE_BROWSER_FILES) $(MAIN_FILE) $(IO_DIR_FILES3) 

CPP_FILES = $(IO_DIR_FILES2) $(SOUND_FILES) $(AUDIO_FILES)

OBJ_FILES = $(addsuffix .o, $(basename $(C_FILES))) $(addsuffix .o, $(basename $(CPP_FILES)))

TARGET		= gameboy.html
OBJS		= $(OBJ_FILES)

CFLAGS		= -O2 -Wall 
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)
PORTS = -s USE_SDL=2 
CC = emcc
CXX = em++



all:
	$(CC)  -c $(CFLAGS) $(C_FILES) $(PORTS) 
	$(CC)  -c $(CXXFLAGS) $(CPP_FILES) $(PORTS) 
	$(CC) $(PORTS) -o $(TARGET) $(OBJ_FILES) -O2 --embed-file rom.gb

clean:
	rm -rf *.o
	rm $(TARGET)
	rm gameboy.js
