![crystal](/images/shantae.png?raw=true) ![dk](/images/ages.png?raw=true)

| Linux                           | OSX                             |
|---------------------------------|---------------------------------|
| [![Build Status](https://travis-ci.org/RossMeikleham/PlutoBoy.svg?branch=master)](https://travis-ci.org/RossMeikleham/GB-Emu-C) | [![Build Status](https://travis-ci.org/RossMeikleham/PlutoBoy.svg?branch=OSX)](https://travis-ci.org/RossMeikleham/GB-Emu-C) |


Gameboy and Gameboy Color emulator for Windows, OSX, Linux/Unix based systems, Web Browsers, and PSP.
Uses SDL for input/output but can easily be expanded to include other frameworks.

The PSP version can be run through the PSP emulator (PPSSPP) for Android/IOS platforms.

Play a demo game in the browser [here](http://rossmeikleham.github.io/GB-Emu-C/).

The emulator supports 2 modes DMG (Dot Matrix Gameboy) and CGB (Color Gameboy),
all original Gameboy games are backwards compatible in CGB mode:

![no_col](/images/bw_logo.png?raw=true)
![col](/images/col_logo.png?raw=true)
![zelda_no_col](/images/nocolor.png?raw=true)
![zelda_mid_col](/images/color1.png?raw=true)
![poke_no_col](/images/blue_nocol.png?raw=true)
![poke_mid_col](/images/blue_col.png?raw=true)

Some Color ROMS are backwards compatible and can also be run in DMG mode:


![poke_no_col](/images/gold_nocol.png?raw=true)
![poke_mid_col](/images/gold_col.png?raw=true)
![mario_no_col](/images/mario_nocol.png?raw=true)
![mario_mid_col](/images/mario_col.png?raw=true)


#Currently Implemented

- Complete instruction set of the Gameboy's modified 
  Z80 processor with correct timings with respect to 
  execution of instructions and memory accesses.
  (according to Blargg's test ROMs)

- Interrupt handling.

- LCD timings and handling.

- Joypad support from Keyboard.

- Serial I/O transfer implemented in TCP to emulate transfer of
  data through the link port. (Experimental) 

- Rendering screen contents after each scanline. After all
  scanlines have been rendered the screen is displayed/updated 
  using SDL.

- Support for regular 32kb, MBC1, MBC3, and MBC5 chip ROMs. (Not including Real Time Clock or Rumble)

- Saving SRAM to and loading SRAM from file for games that support it.

- Sound using Blaarg's C++ sound library which was further edited  by [drhelius]( https://github.com/drhelius). Source files for audio used in this emulator came from his emulator [here](https://github.com/drhelius/Gearboy/tree/master/src/audio)

- Support for most Gameboy Color ROMs 

- Simple debugger which can:  
   - Step through instructions,
   - Set breakpoints,
   - View the contents of memory and registers
   - Disassemble an instruction given the memory address



#Still to implement:

- Support for Real Time Clock for MBC3, rumble for MBC5, and basic support 
  for other MBC ROM chips.

- Updating screen information during the scan line instead
  of all at once when H-Blank occurs. 
  (Needed for some games to render correctly such as Prehistorik Man)

- OAM Sprite bug behaviour

- Save/Load ROM states

- Fixes for graphical glitches and bugs in some ROMS. 

#Using Desktop
`gb_emu_c [options] romfile`
use the `-h` option to display help info

The -d flag starts the emulator in debugging mode.
##Controls:
  - a -> a
  - s -> b
  - enter -> start
  - spacebar -> select
  - arrows keys -> d-pad

#Using PSP
  Select the Gameboy file with "X" to run in cgb mode or "O" to run in dmg mode.
##Controls:
  - X -> A
  - O -> B
  - Start -> Start
  - Select -> Select
  - Joypad uses the PSP Joypad

#Build Instructions

## Linux

### Required: (All these should be easily installed using your package manager)

- SDL 1.2 libraries
- SDL_net library
- Scons 
- Clang compiler 

### How To Build:
- Navigate to the project `build/Unix` folder.
- Enter the command `./scons`
 
Raspberry Pi 1
 - Overclock the Pi to 1 Ghz to be able to run at full speed
 
Raspberry Pi 2
 - Works fine at base cpu speed of 900mhz, if wanting to run more programs at the same time
   and retain full emulation speed, consider overclocking to at least 1Ghz


## OSX

### Required: (All these can be easily installed using the homebrew package manager)

- SDL 1.2 libraries
- SDL_net library
- XCode + XCode Command Line tools 
- Scons

### How To Build:
- Same as for Linux, navigate to the project `build/Unix` folder and enter the command `./scons`

## Windows

### Required:
- Visual C++/ Visual Studio
- SDL 1.2 Visual C++ development libraries (can be found [here](https://www.libsdl.org/download-1.2.php))
- SDL_net Visual C++ development library (can be found [here](https://www.libsdl.org/projects/SDL_net/))
- SDL2.dll (can be found [here] (https://www.libsdl.org/download-2.0.php) Download the VC zip file for Windows.
### How To Build:

- Navigate to the project `build/Windows` folder.

- Open `Windows GB.sdf`

- Navigate to `Project->Properties->VC++ Directories`

- Edit Include directories to include the "include" directory of the downloaded SDL libraries. `SDL-1.2.15/include`
  and `SDL2_net-2.0.0/include`

- Edit Library directories to include a library directory of each the downloaded SDL libraries. `SDL-1.2.15/lib/x86`
  and `SDL2_net-2.0.0/lib/x86`. Can instead include /x64 directory if you wish.

- Press "OK" to close Project Properties dialogue.
- Now navigate to "Build" and select the "Build Solution" option.
- The Windows folder should now contain a release folder containing the emulator executable.

> ***Note*** if SDL.dll and/or SDL2_net.dll from the `lib/x86` SDL folder isn't in your path either add it or copy it to the Release folder. Also from the downloaded SDL2 zip file you downloaded extract either lib/x86/SDL2.DLL or lib/x64/SDL2.DLL into the release folder. 


## Web Browser

### Required:
- Emscripten to compile the C/C++ sources to Javascript

### How to Build:
- Navigate to the `build/Javascript` folder
- Place the rom you want to run in the same folder with the name "rom.gb"
- Enter the command `make`
- gameboy.js and gameboy.html files should be generated.
- To run games, open gameboy.html with a web browser.

### TODO
  - Need to make a way for the user to enter the address of a rom and attempt to load
    it rather than hard coding one at compile time. Also need to add sound.



## PSP

The PSP version doesn't run anywhere near close to full speed, optimisations needs
to be made to the emulator for speeding it up to where it can run on it at full speed.

### Required:

- PSP ToolChain (can be found [here](https://github.com/pspdev/psptoolchain)
- Follow the instructions to install, and ensure all environment variables are set

### How To Build:
- Navigate to the `build/PSP` folder
- Enter the command `make` then `make directory`

If running on PPSSPP emulator
- copy ROMS into the Gameboy folder generated or a subfolder of it. Navigate to the Gameboy folder from PPSSPP and run the program.

If running on an actual PSP
- Copy the Gameboy folder to PSP/GAME folder on the PSP memory stick.
- Upgrade to kernel/firmware version 6.60 or 6.61
- Copy/run the Pro update for either version to run homebrew
- Run the emulator from the PSP



> ***Note*** Psp version will need to be built on a Linux/OSX machine or Windows with Cygwin. Sound is currently disabled in the PSP version, when enabled it temporarily works but freezes after a couple of minutes. 

#Link Cable guide
Currently serial I/O implementation is rather buggy with regards to a few
games, it works perfectly fine with others. It currently only works with
2 emulators on the same machine (localhost), but it's trivial to adjust the ports and the server
the client connects to by editing the server address in the serial SDL file, and recompiling.
I hope to fix the bugs and improve this feature at a later date.

First run an emulator instance as server `gb game [-d] server`, then 
run another emulator instance as a client `gb game [-d] client`.

Each instance of the emulator should now act like it is connected by a link cable,
and you should be able to play some multiplayer games.

![2Player](/images/linkcable.png?raw=true)

#Current Accuracy Test Tesults

![Instructions](/images/instrs.png?raw=true)![Instruction Timing](/images/timing.png?raw=true)![Instructions](/images/mem_timing.png?raw=true)



#Screenshots

![crystal](/images/crystal.png?raw=true) ![dk](/images/dk.png?raw=true)

![Gold](/images/gold.png?raw=true)![zelda_col](/images/zelda_col.png?raw=true)![megaman](/images/megaman.png?raw=true)![warriors](/images/warrior.png?raw=true)

![Tetris](/images/tetris.png?raw=true)![Zelda](/images/zelda.png?raw=true)![Pokemon](/images/poke.png?raw=true)![Mario](/images/mario.png?raw=true)

Browser version running in Firefox
![Firefox](/images/javascript.png?raw=true)

PSP version running on PSP hardware:
![psp](/images/psp.jpg?raw=true)

PSP version Running on PPSSPP emulator on Android:
![psp](/images/pok.png?raw=true)
