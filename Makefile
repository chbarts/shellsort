CC=gcc
CFLAGS=-O3 -march=native -g -flto
DEPS=shellsort.h handle_ferr.h ggets.h
OBJ=shellsort.o handle_ferr.o ggets.o ssort.o

all: ssort

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ssort: ssort.o shellsort.o handle_ferr.o ggets.o
	$(CC) -flto -o ssort ssort.o shellsort.o handle_ferr.o ggets.o

clean:
	rm ssort $(OBJ)
