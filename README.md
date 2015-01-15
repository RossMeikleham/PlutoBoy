
| Linux                           | OSX                             |
|---------------------------------|---------------------------------|
| [![Build Status](https://travis-ci.org/RossMeikleham/GB-Emu-C.svg?branch=master)](https://travis-ci.org/RossMeikleham/GB-Emu-C) | [![Build Status](https://travis-ci.org/RossMeikleham/GB-Emu-C.svg?branch=OSX)](https://travis-ci.org/RossMeikleham/GB-Emu-C) |


Gameboy emulator for Windows, OSX, PSP, and Linux/Unix based systems.
Uses SDL for input/output but can easily be expanded to include other frameworks.


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

#Using 
Windows command line: `gb_emu.exe "game_file" [-d] [client/server]`
<br>
Unix shell: `./gb_emu "game_file" [-d] [client/server]` 
<br>
The -d flag starts the emulator in debugging mode.


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

## PSP (Currently in development, builds but is missing implementation of most IO features required to run games)

### Required:

- PSP ToolChain (can be found [here](https://github.com/pspdev/psptoolchain)
  Follow the instructions to install, and ensure all environment variables are set

### How To Build:
- Navigate to the `build/PSP` folder
- Enter the command `make`
- `Eboot.pbp` file should be generated in the folder, copy this file onto your PSP and run it from there


> ***Note*** Will need to be built on a Linux/OSX machine or Windows with Cygwin

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

![Tetris](/images/tetris.png?raw=true)![Zelda](/images/zelda.png?raw=true)![Pokemon](/images/poke.png?raw=true)![Mario](/images/mario.png?raw=true)
