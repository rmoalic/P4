CC=clang
LDFLAGS+=-lSDL2 -lSDL2_ttf
CFLAGS+=-std=c11 -g -Wall -Wextra -pedantic -I/usr/include/SDL2
#CFLAGS+=-O3 -Wall -Wextra -s -DNDEBUG -pedantic

all: bin

bin: main

dist: bin
	mkdir out
	cp main out/
	cp *.ttf out/

main: main.c p4.c p4.h layout.c list.o
	$(CC) $(CFLAGS) $(LDFLAGS) main.c p4.c layout.c list.o -o $@

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

clean:
	rm main

.PHONY: clean
