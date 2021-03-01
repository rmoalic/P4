CC=clang
LDFLAGS+=-lSDL2 -lSDL2_ttf
CFLAGS+=-g -Wall -Wextra -pedantic
#CFLAGS+=-O3 -Wall -Wextra -s -DNDEBUG -pedantic

all: bin

bin: main

main: main.c p4.c p4.h
	$(CC) $(CFLAGS) $(LDFLAGS) main.c p4.c -o $@

clean:
	rm main

.PHONY: clean
