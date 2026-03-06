CC=gcc

all: build

build: main.c asmdef.h asmdef.c
	$(CC) -Os -Wl,--gc-sections -c -Wall -Wextra asmdef.c -o asmdef.o
	$(CC) -Os -Wl,--gc-sections -c -Wall -Wextra syscallof.c -o syscallof.o
	$(CC) -Os -Wl,--gc-sections -Wall -Wextra main.c asmdef.o syscallof.o -o syscallof

clean:
	rm -f syscallof.o asmdef.o syscallof
