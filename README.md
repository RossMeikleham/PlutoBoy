# Plutoboy 

A multiplatform Gameboy and Gameboy Color emulator for Windows, OSX, Linux/Unix based systems, Android, iOS, UEFI, Web Browsers, PSP, PSP Vita, 3DS, and Switch.

![crystal](/images/shantae.png?raw=true) ![dk](/images/ages.png?raw=true)


| Platform   | CI Build Status | SDL 1.2 | SDL 2 | Native API |
|------------|-----------------|---------|-------|-----|
| ![linuxIcon](/images/linuxIcon.png?raw=true) Linux |![BuildStatus](https://github.com/RossMeikleham/Plutoboy/workflows/Linux/badge.svg?branch=master) | ✔ | ✔ | N/A |
| ![piIcon](/images/piIcon.png?raw=true) Raspberry Pi |![BuildStatus](https://github.com/RossMeikleham/Plutoboy/workflows/Linux/badge.svg?branch=master) | ✗ | ✔ | N/A |
| ![osxIcon](/images/osxIcon.png?raw=true) macOS | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/macOS/badge.svg?branch=master) | ✔ | ✔ | N/A |
| ![windowsIcon](/images/windowsIcon.png?raw=true) Windows | ![Build status](https://github.com/RossMeikleham/Plutoboy/workflows/Windows/badge.svg?branch=master)| ✗  | ✔ | N/A | 
|![androidIcon](/images/androidIcon.jpeg?raw=true) Android | ![BuildStatus](https://github.com/RossMeikleham/Plutoboy/workflows/Android/badge.svg?branch=master) | ✗ | ✔ | N/A |
| ![iosIcon](/images/apple.png?raw=true) iOS | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/iOS/badge.svg?branch=master) | ✗ | ✔ | N/A |
| ![watchosIcon](/images/watch.png?raw=true) watchOS | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/watchOS/badge.svg?branch=master) | ✗ | ✗ | ✔ |
| ![uefiIcon](/images/uefi.png?raw=true) UEFI | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/UEFI/badge.svg?branch=master) | ✗ | ✗ | ✔ |
| ![webasmIcon](/images/webasmIcon.png?raw=true) WebASM | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/WebASM/badge.svg?branch=master) | ✗ | ✔ | N/A|
| ![pspIcon](/images/pspIcon.png?raw=true) PSP | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/PSP/badge.svg?branch=master) | ✔ | ✗ | N/A |
| ![dreamIcon](/images/dreamIcon.png?raw=true) Dreamcast | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/Dreamcast/badge.svg?branch=master) | ✔ | ✗ | N/A |
| ![VitaIcon](/images/ps-vita.png?raw=true) PS Vita | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/Vita/badge.svg?branch=master) | ✗ | ✔ | N/A |
| ![3dsIcon](/images/3ds.png?raw=true) 3DS | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/3DS/badge.svg?branch=master) | ✔ | ✗ | N/A |
| ![switchIcon](/images/switch.png?raw=true) Switch | ![Build Status](https://github.com/RossMeikleham/Plutoboy/workflows/Switch/badge.svg?branch=master) | ✗ | ✔ | N/A |

Play in your web browser [here](http://rossmeikleham.github.io/PlutoBoy/), the game running is an open source Gameboy Color game called [µCity](https://github.com/AntonioND/ucity). Controls are the same as the Desktop controls (see below).

The emulator supports 2 modes DMG (Dot Matrix Gameboy) and CGB (Color Gameboy),
all original Gameboy games are backwards compatible in CGB mode:

![no_col](/images/bw_logo.png?raw=true)
![col](/images/col_logo.png?raw=true)
![zelda_no_col](/images/nocolor.png?raw=true)
![zelda_mid_col](/images/color1.png?raw=true)
![poke_no_col](/images/blue_nocol.png?raw=true)
![poke_mid_col](/images/blue_col.png?raw=true)

Some Color ROMS can also be run in DMG mode:


![poke_no_col](/images/gold_nocol.png?raw=true)
![poke_mid_col](/images/gold_col.png?raw=true)
![mario_no_col](/images/mario_nocol.png?raw=true)
![mario_mid_col](/images/mario_col.png?raw=true)


# Currently Implemented

- Complete instruction set of the Gameboy's Sharp LR35902 processor with correct timings with respect to 
  execution of instructions and memory accesses.
  (according to Blargg's test ROMs)

- Interrupt handling.

- LCD timings and handling.

- Joypad support from Keyboard, and touch screen
  controls when using SDL2.

- Serial I/O transfer implemented in TCP to emulate transfer of
  data through the link port. (Experimental) 

- Rendering screen contents after each scanline. After all
  scanlines have been rendered the screen is displayed/updated 
  using SDL.

- Support for regular 32KB, MBC1, MBC2, MBC3, MBC5, HuC1, HuC3, and MMM01 chip ROMs. (Not including Real Time Clock or Rumble)

- Saving SRAM to and loading SRAM from file for games that support it.

- Sound using Blaarg's C++ sound library which was further edited  by [drhelius]( https://github.com/drhelius). Source files for audio used in this emulator came from his emulator [here](https://github.com/drhelius/Gearboy/tree/master/src/audio)

- Support for most Gameboy Color ROMs 

- Simple debugger (for desktop platforms) which can:  
   - Step through instructions,
   - Set breakpoints,
   - View the contents of memory and registers
   - Disassemble an instruction given the memory address



# Still to implement:

- Support for Real Time Clock for MBC3 + HuC3, rumble for MBC5. 
  MBC6, MBC7, TAMA5, and Gameboy Camera Memory Controllers.

- Updating screen information during the scan line instead
  of all at once when H-Blank occurs. 
  (Needed for some games to render correctly such as Prehistorik Man)

- OAM Sprite bug behaviour

- Save/Load ROM states

- Fixes for graphical glitches and bugs in some ROMS. 

# Using Desktop
`plutoboy [options] romfile`
use the `-h` option to display help info

The -d flag starts the emulator in debugging mode.
## Controls:
  - a -> a
  - s -> b
  - enter -> start
  - spacebar -> select
  - arrows keys -> d-pad

# Using PSP
  Select the Gameboy file with "X" to run in cgb mode or "O" to run in dmg mode.
## Controls:
  - X -> A
  - O -> B
  - Start -> Start
  - Select -> Select
  - Joypad uses the PSP Joypad

# Build Instructions

Github Actions CI produces builds for most platforms, see https://github.com/RossMeikleham/PlutoBoy/actions, and click on the workflow for the desired platform, and then select "Artifacts". The CI is scheduled to run once a week and after any pushes.

## Linux

### Building for x86 using Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_linux -f build/Unix/x86/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_linux
```

### Building for ARM/Raspberry Pi using Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_linux_arm -f build/Unix/ARM/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_linux_arm
```

### Building from scratch

#### Required: (All these should be easily installed using the package manager provided by your Linux distribution)

- SDL2  
- SDL2_net 
- Scons 
- Clang or GCC 

#### How To Build:
- Navigate to the project `build/Unix` folder.
- Enter the command `./scons cc=[compiler]` where `[compiler]` is either "gcc" or "clang". Leaving out the cc option compiles with clang by default.
 
### Notes 

#### Raspberry Pi 1
 - Overclock the Pi to 1 Ghz to be able to run at full speed
 
#### Raspberry Pi 2
 - Works fine at base cpu speed of 900mhz, if wanting to run more programs at the same time
   and retain full emulation speed, consider overclocking to at least 1Ghz


## OSX

### Required: (All these can be easily installed using the homebrew package manager)

- SDL_2 
- SDL2_net 
- XCode + XCode Command Line tools 
- Scons

### How To Build:
- Same as for Linux, navigate to the project `build/Unix` folder and enter the command `./scons`

## Windows 
### Required:
- MinGW (Easiest way is to install via Choclatey)
- Scons (Install via Python3 pip)

### How To Build:

- Navigate to the project `build/Windows/scons` folder.
 
- Enter the command `scons`

- The scons builder should download the required SDL dependencies and build the emulator,
  the executable along with the required SDL DLLs should be in a new folder called `build` 
  inside the current `scons` folder

> ***Note*** if SDL.dll and/or SDL2_net.dll from the `lib/x86` SDL folder isn't in your path either add it or copy it to the Release folder. Also from the downloaded SDL2 zip file you downloaded extract either lib/x86/SDL2.DLL or lib/x64/SDL2.DLL into the release folder. 

## Android

### Building using Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_android -f build/Android/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_android
```

### Building from scratch

See the steps the Dockerfile takes

## iOS
### Required:
- XCode 12

### How To Build + Run

- Setup the SDL2 iOS framework by generating the `SDL2.Framework`, `SDL2_ttf.Framework`, and `SDL2_image.Framework` folders from [here](https://github.com/manifest/sdl-ios-framework)
- Copy `SDL2.Framework`, `SDL2_ttf.Framework`, and `SDL2_image.Framework` to `~/Library/Frameworks/`. Create the `Frameworks` folder if it doesn't exist.
- Open `build\IOS\Plutoboy.xcodeproj` in XCode, in Targets -> Plutoboy -> Build Phases, add your roms to the "Copy Bundle Resources" items.
- In XCode select the build target (either simulator or plugged in device). If using an actual device you will need to setup the codesigning options to point to a free or paid for developer account. Then click run.

# TODO
    - Setup shared folders so saves and ROMS can be accessed via iCloud instead of compiling in the ROMs at build time.


## watchOS


| ![selectImage](/images/watchOS_fileSelect.png | width=250) | ![gameplayImage](/images/watchOS_gamePlay.png | width=250) |

### Required:
- XCode 12

### How To Build + Run
- In Xcode remove "test_rom.gb" and "test_rom.png" and add your rom files to the project in the same location, either with the extension of ".gb" or ".gbc". For each rom you can also optionally add a png image of the name <rom_name>.png for the image to show up in the rom selection when running the app.
- In XCode select the build target (either simulator or plugged in device). If using an actual device you will need to setup the codesigning options to point to a free or paid for developer account. Then click run.

# TODO
    - Setup shared folders so saves and ROMS can be accessed via iCloud instead of compiling in the ROMs at build time.
    - Fix sound, I made the mistake of attempting to use AudioKit which isn't fit for the purpose of outputting sound samples at the rate emulation expects which causes pretty bad stuttering. Need to convert to using CoreAudio like SDL does on macOS and iOS platforms. 


## Web Assembly

### Building using Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
cp Path/To/GameboyRom.gb build/WebAsm/rom.gb
docker build -t plutoboy_webasm -f build/WebAsm/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_webasm

cd WebAsmFiles
docker run -dit --name plutoboy_webasm_app -p 8080:80 -v "$PWD":/usr/local/apache2/htdocs/ httpd:2.4
```

Then navigate to "http://localhost:8080/" in a Web Browser.

### Building from scratch
#### Required:
- [Emscripten]( https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html) to compile the C/C++ sources to Web Assembly

#### How to Build:
- Navigate to the `build/Javascript` folder
- Place the rom you want to run in the same folder with the name "rom.gb"
- Enter the command `make`
- plutoboy.js, plutoboy.webasm, and plutoboy.html files should be generated.
- To run games, open plutoboy.html with a web browser.
- In Google Chrome you may need to close all instances of chrome and then start a new one from the terminal/command line with the flag `--allow-file-access-from-files`.

### TODO
  - Need to make a way for the user to enter the address of a rom and attempt to load
    it rather than hard coding one at compile time.  


## PSP

The PSP version doesn't run anywhere near close to full speed, optimisations needs
to be made to the emulator for speeding it up to where it can run on it at full speed.

### Building using Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_psp -f build/PSP/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_psp
```

### Building from scratch
#### Required:

- PSP ToolChain (can be found [here](https://github.com/pspdev/psptoolchain)
- PSP Libraries (can be found [here](https://github.com/pspdev/psplibraries)

#### How To Build:
- Navigate to the `build/PSP` folder
- Enter the command `make` then `make directory`

## Running 
#### If running on PPSSPP emulator
- copy ROMS into the Plutoboy folder generated or a subfolder of it. Navigate to the Plutoboy folder from PPSSPP and run the program.

#### If running on an actual PSP
- Copy the Gameboy folder to PSP/GAME folder on the PSP memory stick.
- Upgrade to kernel/firmware version 6.60 or 6.61
- Copy/run the Pro update for either version to run homebrew
- Run the emulator from the PSP


> ***Note*** Sound is currently disabled in the PSP version, when enabled it temporarily works but freezes after a couple of minutes. 

## PSP Vita

### Building VPK with Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_vita -f build/Vita/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_vita
```

### Building VPK from scratch

Requires installing the [VitaSDK toolchain](https://vitasdk.org/)

```
cd build/Vita
mkdir build
cd build
cmake ..
make
```

# 3DS

### Building with Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_3ds -f build/3DS/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_3ds
```

This should produce a `Plutoboy.3dsx` file and a `Plutoboy.cia` file.

The 3dsx file can be loaded through the 3ds homebrew launcher using the netloader (press Y in the app), and use the 3dslink program to transfer the app.

The cia file can be installed onto the 3DS using the fbi app installer.

It should run at the full 59.7 fps on the new 2DS/3DS, with the original 2DS/3DS it runs very slow.

R button turns the frame limiter off, L button turns it back on.

File I/O for some reason is very slow with 3DS homebrew, games which rapidly save to SRAM and turn it off may slowdown for a while due to this.

# Switch (Requires CFW to run)

### Building with Docker
```
git clone https://github.com/RossMeikleham/PlutoBoy
cd Plutoboy
docker build -t plutoboy_switch -f build/Switch/Dockerfile .
docker run -v $(pwd):/mnt plutoboy_switch
```

This should produce a `Plutoboy.nro` file.

The nro file can be loaded through the Switch homebrew menu using the netloader (press Y in the app), and use the nxlink program to transfer the app.


# Link Cable guide (Desktop platforms Windows/Linux/OSX)
Currently serial I/O implementation is rather buggy with regards to a few
games, it works perfectly fine with others. It currently only works with
2 emulators on the same machine (localhost), but it's trivial to adjust the ports and the server
the client connects to by editing the server address in the serial SDL file, and recompiling.
I hope to fix the bugs and improve this feature at a later date. (Synchronization needs fixed)

First run an emulator instance as server `plutoboy game [-d] server`, then 
run another emulator instance as a client `plutoboy game [-d] client`.

Each instance of the emulator should now act like it is connected by a link cable,
and you should be able to play some multiplayer games.

![2Player](/images/linkcable.png?raw=true)

# Current Accuracy Test Tesults

![Instructions](/images/instrs.png?raw=true)![Instruction Timing](/images/timing.png?raw=true)![Instructions](/images/mem_timing.png?raw=true)



# Screenshots

![crystal](/images/crystal.png?raw=true) ![dk](/images/dk.png?raw=true)

![Gold](/images/gold.png?raw=true)![zelda_col](/images/zelda_col.png?raw=true)![megaman](/images/megaman.png?raw=true)![warriors](/images/warrior.png?raw=true)

![Tetris](/images/tetris.png?raw=true)![Zelda](/images/zelda.png?raw=true)![Pokemon](/images/poke.png?raw=true)![Mario](/images/mario.png?raw=true)

Browser version running in Firefox
![Firefox](/images/javascript.png?raw=true)

PSP version running on PSP hardware:
![psp](/images/pspray.jpg?raw=true)
![psp](/images/psp.jpg?raw=true)

PSP version Running on PPSSPP emulator on Android:
![psp](/images/pok.png?raw=true)
