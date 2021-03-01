CC=clang
LDFLAGS+=-lSDL2 -lSDL2_ttf
CFLAGS+=-g -Wall -Wextra -pedantic
#CFLAGS+=-O3 -Wall -Wextra -s -DNDEBUG -pedantic

all: bin

bin: main

main: main.c p4.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm main

.PHONY: clean
