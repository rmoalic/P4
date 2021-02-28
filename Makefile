all: bin

bin: main

main: main.c p4.c
	clang -g -Wall -Wextra -lSDL2 main.c p4.c -o main
