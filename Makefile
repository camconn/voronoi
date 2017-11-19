# Simple freaking makefile

HEADERS = voronoi.h colors.h

default: voronoi

voronoi: voronoi.c $(HEADERS)
	gcc -g -lm -lpng voronoi.c -o voronoi -std=c99

clean:
	rm -f voronoi
