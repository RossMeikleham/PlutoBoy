MAIN_DIR=../../src/platforms/Dreamcast
COMMON_DIR = $(MAIN_DIR)/common
CORE_DIR=../../src/core
MMU_DIR=$(CORE_DIR)/mmu
IO_DIR=../../src/shared_libs/SDL
SOUND_DIR=../../src/shared_libs/SDL/audio

MAIN_DIR_FILES = $(wildcard $(MAIN_DIR)/*.c)
COMMON_DIR_FILES = $(wildcard $(COMMON_DIR)/*.c)
CORE_DIR_FILES = $(wildcard $(CORE_DIR)/*.c)
MMU_DIR_FILES = $(wildcard $(MMU_DIR)/*.c)
IO_DIR_FILES = $(wildcard $(IO_DIR)/*.c)
IO_DIR_FILES2 = $(wildcard $(IO_DIR)/*.cpp)
SOUND_FILES = $(wildcard $(SOUND_DIR)/*.cpp)

C_FILES = $(MAIN_DIR_FILES) $(COMMON_DIR_FILES) $(CORE_DIR_FILES) $(MMU_DIR_FILES) $(IO_DIR_FILES) $(IO_DIR_FILES2) $(SOUND_FILES)
OBJ_FILES = $(addsuffix .o, $(basename $(C_FILES)))

TARGET = gameboy

OPTFLAGS=-std=c99 -O3 -fomit-frame-pointer
CFLAGS = -DDREAMCAST
KOS_CFLAGS+= -I$(KOS_PORTS)/include/SDL $(OPTFLAGS) 
KOS_ROMDISK_DIR = romdisk

all: rm_romdisk $(TARGET).bin

include $(KOS_BASE)/Makefile.rules

.SRCS	=	$(C_FILES) \

OBJS = $(OBJ_FILES) romdisk.o

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin romdisk.*


rm_romdisk:
	-rm -f $(TARGET) romdisk.*

$(TARGET).elf: $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET).elf $(KOS_START) \
		$(OBJS) -lSDL -lm $(OBJEXTRA) $(KOS_LIBS)


#$(OBJS) -L$(KOS_PORTS)/SDL/inst/lib -lSDL -lm $(OBJEXTRA) $(KOS_LIBS)

$(TARGET).bin: $(TARGET).elf
	$(KOS_OBJCOPY) -R .stack -O binary $(TARGET).elf $(TARGET).bin

run: $(TARGET).bin
	$(KOS_LOADER) $(TARGET).bin

