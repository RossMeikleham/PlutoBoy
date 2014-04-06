CFLAGS = -g
LINKS = -lpthread -lSDL -lX11 

all: GameBoy clean

clean:
	rm *.o

GameBoy: cpu.o main.o memory.o graphics.o IO.o
	gcc -o GameBoy $(CFLAGS) cpu.o memory.o graphics.o IO.o main.o $(LINKS)

cpu.o: cpu.c memory.h cpu.h
	gcc -c $(CFLAGS) cpu.c

memory.o: memory.c
	gcc -c $(CFLAGS) memory.c

main.o: main.c
	gcc -c $(CFLAGS) main.c

graphics.o: graphics.c
	gcc -c $(CFLAGS) graphics.c

IO.o: IO.c
	gcc -c $(CFLAGS) IO.c
