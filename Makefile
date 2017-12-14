CC=gcc
CFLAGS=-O3 -march=native -g -flto
DEPS=shellsort.h handle_ferr.h ggets.h
OBJ=shellsort.o handle_ferr.o ggets.o ssort.o
OBJ32=shellsort32.o handle_ferr32.o ggets32.o ssort32.o

all: ssort

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%32.o: %.c $(DEPS)
	$(CC) -m32 -c -o $@ $< $(CFLAGS)

ssort: ssort.o shellsort.o handle_ferr.o ggets.o
	$(CC) -flto -o ssort ssort.o shellsort.o handle_ferr.o ggets.o

ssort32: ssort32.o shellsort32.o handle_ferr32.o ggets32.o
	$(CC) -flto -m32 -o ssort32 ssort32.o shellsort32.o handle_ferr32.o ggets32.o

clean:
	rm ssort $(OBJ)

clean32:
	rm ssort32 $(OBJ32)
