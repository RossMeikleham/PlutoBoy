CFLAGS = -g -W -Wall
LINKS = -lSDL -lX11 
COMPILER = clang

VPATH=core:desktop

all: GameBoy clean

clean:
	rm *.o

GameBoy: cpu.o memory.o graphics.o disasm.o IO.o 
	$(COMPILER) -o GameBoy $(CFLAGS) disasm.o cpu.o memory.o graphics.o IO.o main.o $(LINKS)

cpu.o: cpu.c memory.h cpu.h
	$(COMPILER) -c $(CFLAGS) cpu.c

memory.o: memory.c
	$(COMPILER) -c $(CFLAGS) memory.c

main.o: main.c
	$(COMPILER) -c $(CFLAGS) main.c

graphics.o: graphics.c
	$(COMPILER) -c $(CFLAGS) graphics.c

IO.o: IO.c
	$(COMPILER) -c $(CFLAGS) IO.c

disasm.o : disasm.c
	$(COMPILER) -c $(CFLAGS) disasm.c
