CFLAGS = -g

all: GameBoy clean

clean:
	rm *.o

GameBoy: cpu.o main.o
	gcc -o $(CFLAGS) Z80.o main.o

cpu.o: Z80.c memory.h cpu.h
	gcc -c $(CFLAGS) Z80.c

main.o:
	gcc -c $(CFLAGS) main.c
