cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

set(SHORT_NAME Plutoboy)
project(${SHORT_NAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)
add_definitions(-DPSVITA)

set(VITA_APP_NAME "Plutoboy")
set(VITA_TITLEID  "PLUTO0013")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O22 -std=gnu11 -mtune=cortex-a9")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -mtune=cortex-a9")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")

include_directories(
  ../common
)

file(GLOB plutoboy_src
    "../../src/core/*.c"
    "../../src/core/*.h"
    "../../src/core/mmu/*.c"
    "../../src/core/mmu/*.h"
    "../../src/core/audio/*.cpp"
    "../../src/core/audio/*.h"
    "../../src/non_core/*.h"
    "../../src/platforms/Vita/*.c"
    "../../src/platforms/Vita/*.h"
    "../../src/shared_libs/SDL2/UI/*.c" 
    "../../src/shared_libs/SDL2/*.c"
    "../../src/shared_libs/SDL2/*.cpp"
    "../../src/shared_libs/SDL2/audio/*.h" 
    "../../src/shared_libs/SDL2/audio/*.cpp"
    "../../src/platforms/Vita/file_browser/filebrowser.c"
    "../../src/shared_libs/*.c"
)

add_executable(${SHORT_NAME} ${plutoboy_src})

target_link_libraries(${SHORT_NAME}
  SDL2
  SDL2_ttf
  SDL2_image
  png
  jpeg
  bz2
  freetype
  z 
  vita2d
  SceDisplay_stub
  SceCtrl_stub
  SceAudio_stub
  SceSysmodule_stub
  SceGxm_stub
  SceCommonDialog_stub
  SceAppMgr_stub
  SceMotion_stub
  SceTouch_stub
  ScePower_stub
  SceHid_stub
  SceAppMgr_stub
  m 
)

vita_create_self(${SHORT_NAME}.self ${SHORT_NAME})
vita_create_vpk(${SHORT_NAME}.vpk ${VITA_TITLEID} ${SHORT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE assets/sce_sys/icon0.png sce_sys/icon0.png
  FILE assets/sce_sys/livearea/contents/bg0.png sce_sys/livearea/contents/bg0.png
  FILE assets/sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE assets/sce_sys/livearea/contents/frame_icon.png sce_sys/livearea/contents/frame_icon.png
  FILE assets/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE assets/8_Bit_Madness.ttf 8_Bit_Madness.ttf
)
