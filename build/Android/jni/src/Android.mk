LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

# Get paths for all the emulator's source files
SRC_ROOT := ../../../../src

MAIN_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/platforms/Android
CORE_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/core
MMU_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/core/mmu
AUDIO_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/core/audio
SDL2_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/shared_libs/SDL2
SDL2_AUDIO_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/shared_libs/SDL2/audio
SDL2_AUDIO_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/shared_libs/SDL2/audio
SHARED_DIR := $(LOCAL_PATH)/$(SRC_ROOT)/shared_libs

MAIN_FILES := $(wildcard $(MAIN_DIR)/*.c)
CORE_FILES = $(wildcard $(CORE_DIR)/*.c)
MMU_FILES = $(wildcard $(MMU_DIR)/*.c)
AUDIO_FILES = $(wildcard $(AUDIO_DIR)/*.cpp)
SDL2_FILES = $(wildcard $(SDL2_DIR)/*.c) 
SDL2_CPP_FILES = $(wildcard $(SDL2_DIR)/*.cpp) 
SDL2_AUDIO_FILES = $(wildcard $(SDL2_AUDIO_DIR)/*.cpp)
EXTRA_FILES = $(wildcard $(SHARED_DIR)/*.c)

GB_FILES = $(MAIN_FILES) $(CORE_FILES) $(MMU_FILES) $(AUDIO_FILES) $(SDL2_FILES) $(SDL2_CPP_FILES) $(SDL2_AUDIO_FILES) $(EXTRA_FILES)

LOCAL_CFLAGS += -std=c99 -O2
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c $(GB_FILES:$(LOCAL_PATH)/%=%) 


LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
