Gameboy emulator for Windows, OSX and Linux/Unix based systems.
Uses SDL for input/output but can easily be expanded to include other frameworks.


#Currently Implemented

- Complete instruction set of the Gameboy's modified 
  Z80 processor with correct timings with respect to 
  execution of instructions and memory accesses.
  (according to Blargg's test ROMs)

- Interrupt handling.

- LCD timings and handling.

- Joypad support from Keyboard.

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

- Serial data transfer handling.

- Updating screen information during the scan line instead
  of all at once when H-Blank occurs. 
  (Needed for some games to render correctly such as Prehistorik Man)

- OAM Sprite bug behaviour

- Save/Load ROM states

#Using 
Windows command line: `gb_emu.exe "game_file" [-d]`
<br>
Unix shell: `./gb_emu "game_file" [-d]`
<br>
The -d flag starts the emulator in debugging mode.


#Build Instructions

## Linux

### Required: (All these should be easily installed using your package manager)

- SDL 1.2 libraries
- Scons 
- Clang compiler 

### How To Build:
- Navigate to the project `build/Unix` folder.
- Enter the command `./scons`


## OSX

### Required: (All these can be easily installed using the homebrew package manager)

- SDL 1.2 libraries
- XCode + XCode Command Line tools 
- Scons

### How To Build:
-- Same as for Linux, navigate to the project `build/Unix` folder and enter the command `./scons`

## Windows

### Required:
- Visual C++/ Visual Studio
- SDL 1.2 Visual C++ development libraries (can be found [here](https://www.libsdl.org/download-1.2.php))

### How To Build:

- Navigate to the project `build/Windows` folder.

- Open `Windows GB.sdf`

- Navigate to `Project->Properties->VC++ Directories`

- Edit Include directories to include the "include" directory of the downloaded SDL libraries. `SDL-1.2.15/include`

- Edit Library directories to include a library directory of the downloaded SDL libraries. `SDL-1.2.15/lib/x86`

- Press "OK" to close Project Properties dialogue.
- Now navigate to "Build" and select the "Build Solution" option.
- The Windows folder should now contain a release folder containing the emulator executable.

> ***Note*** if SDL.dll from the `lib/x86` SDL folder isn't in your path either add it or copy it to the Release folder

#Current Accuracy Test Tesults

![Instructions](/images/instrs.png?raw=true)![Instruction Timing](/images/timing.png?raw=true)![Instructions](/images/mem_timing.png?raw=true)



#Screenshots

![Tetris](/images/tetris.png?raw=true)![Zelda](/images/zelda.png?raw=true)![Pokemon](/images/poke.png?raw=true)![Mario](/images/mario.png?raw=true)
