# 3DES
A (simple) homebrew 3ds File Browser written in C

 - Top screen - main file Browser
 - Upper area of bottom screen - instructions
 - Lower area of bottom screen - debug output (look here if, for example, you try to create a directory and nothing happens)

### Buttons:

 - A - Change to selected directory / Open file context menu
 - B - go "up" a directory / go back
 - X - CD to `/`
 - L - Create a new directory (brings a keyboard up to type the name)
 - R - Delete file/directory
 - DPAD/Circle pad up and down control the selection of files/directories
 - DPAD/Circle pad left and right go to the top/bottom of the file/directory list
 - START to close app and go back to HB menu

### Warnings

- Deleting directories uses a recursive function, so if you have a large (very large) amount of subdirectories inside the directory you want to delete, it may end up breaking


# Credits

Compiled with [devkitPro](https://devkitpro.org/)

`sha256.c` & `sha256.h` created by [Brad Conte](https://github.com/B-Con/crypto-algorithms), edited by me

Thanks to [Pirater12](https://github.com/Pirater12) and [LiquidFenrir](https://github.com/LiquidFenrir) for helping me
with the code :)


# Notes (mainly for myself)

[Directory stuff](https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html) | [Using escape codes](https://smealum.github.io/ctrulib/graphics_2printing_2colored-text_2source_2main_8c-example.html#a1) | [Explanation of `strtok()`](http://stackoverflow.com/a/3890186)

- According to Wikipedia, the max FAT32 path length is 255 UTF-16 characters, so size of current_path = 255 * 2 = 510 (because the 16 in UTF-16 means 16 bits = 2 bytes)
- All malloc / realloc statements + arrays create the length needed +1, because there needs to be room for the end character

### ToDo

 - Finish context menu
   + Copy, Cut & Paste
   + Show file size
   + stat() info
- Add create dummy file option
- Add ability to view file hex / text
- Custom font / font-size (if possible?)
- Add ability to rename file / directory
- Add ability to mark files for multiple deletion and/or copying
- Look into accessing areas other than `sdmc:/`

### Build

 - `make`
 - `makerom -f cia -o 3DES.cia -rsf meta\cia.rsf -target t -exefslogo -elf 3DES.elf -icon 3DES.smdh -banner meta\banner.bin`
