GameBoyEmulator
==============

Gameboy emulator in progress.
Written in C.



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

- Support for basic 32kb and MBC1 chip ROMs

- Simple debugger which can:  
   - Step through instructions,
   - Set breakpoints,
   - View the contents of memory and registers
   - Disassemble an instruction given the memory address



#Still to implement:

- Saving SRAM to and loading SRAM from file for games
  that support it.

- Support for all other MBC ROM chips.

- Correct Sprite priorities when overlapping occurs 
  as well as limiting 10 sprites to a line.

- Serial data transfer handling.

- Updating screen information during the scan line instead
  of all at once when H-Blank occurs. 
  (Needed for some games to render correctly such as Prehistorik Man)

- OAM Sprite bug behaviour

- Sound

- Save/Load ROM states

#Using 
From windows command line: 
gb_emu.exe "game_file" [-d]

Unix shell:
./gb_emu "game_file" [-d]

The -d flag starts the emulator in debugging mode.


#Build Instructions

#Unix based systems

Required: (All these should be easily installed using your package manager)
-SDL 1.2 libraries
-Scons 
-Clang compiler 

Navigate to the project build/Unix folder.
Enter the command "./scons"


#Windows
Required:
-Visual C++
-SDL 1.2 Visual C++ development libraries (can be found here https://www.libsdl.org/download-1.2.php)

Navigate to the project build/Windows folder.
Open "Windows GB.sdf".
Navigate to Project->Properties->VC++ Directories
Edit Include directories to include the "include" directory of the downloaded SDL libraries. (SDL-1.2.15/include)
Edit Library directories to include a library directory of the downloaded SDL libraries. (SDL-1.2.15/lib/x86) 
Press "OK" to close Project Properties dialogue.
Now navigate to "Build" and select the "Build Solution" option.
The Windows folder should now contain a release folder containing the emulator executable.
(Note if SDL.dll from either the lib/x86 SDL folder isn't in your path either add it or copy it to the Release folder)

#Current Accuracy Test Tesults

![Instructions](/images/instrs.png?raw=true)![Instruction Timing](/images/timing.png?raw=true)![Instructions](/images/mem_timing.png?raw=true)



#Screenshots

![Tetris](/images/tetris.png?raw=true)![Zelda](/images/zelda.png?raw=true)![Pokemon](/images/poke.png?raw=true)![Mario](/images/mario.png?raw=true)
