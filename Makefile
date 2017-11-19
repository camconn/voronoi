# Simple freaking makefile

CC=gcc
CFLAGS=-Wall -Wpedantic -std=gnu11 -O2
LDFLAGS=-lm -lpng
OBJECTS = voronoi.o colors.o
DEFAULT=voronoi

all: voronoi

debug: clean
debug: CFLAGS += -DDEBUG -g
debug: voronoi

voronoi: $(OBJECTS)
voronoi.o: voronoi.c voronoi.h
colors.o: colors.c colors.h

clean:
	rm -f voronoi
	rm -f *.o
