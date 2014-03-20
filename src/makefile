CFLAGS = -g
LINKS = -lpthread

all: GameBoy clean

clean:
	rm *.o

GameBoy: cpu.o main.o memory.o
	gcc -o GameBoy $(CFLAGS) cpu.o memory.o main.o $(LINKS)

cpu.o: cpu.c memory.h cpu.h
	gcc -c $(CFLAGS) cpu.c

memory.o: memory.c
	gcc -c $(CFLAGS) memory.c

main.o: main.c
	gcc -c $(CFLAGS) main.c
