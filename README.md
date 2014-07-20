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



#Current Accuracy Test Tesults

![Instructions](/images/instrs.png?raw=true)![Instruction Timing](/images/timing.png?raw=true)![Instructions](/images/mem_timing.png?raw=true)



#Screenshots

![Tetris](/images/tetris.png?raw=true)![Zelda](/images/zelda.png?raw=true)![Pokemon](/images/poke.png?raw=true)![Mario](/images/mario.png?raw=true)
