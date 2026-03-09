CC=gcc
FLAGS= -Wall -Wextra

all: build

build: main.c asmdef.h asmdef.c
	$(CC) -c $(FLAGS) asmdef.c -o asmdef.o
	$(CC) -c $(FLAGS) syscallof.c -o syscallof.o
	$(CC) $(FLAGS) main.c asmdef.o syscallof.o -o syscallof

clean:
	rm -f syscallof.o asmdef.o syscallof
