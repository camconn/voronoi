default: voronoi

voronoi.o: voronoi.c
	gcc -c voronoi.c -o voronoi.o -lm -std=c99 -Wall

voronoi: voronoi.o
	gcc voronoi.o -o voronoi -lm -std=c99 -Wall

clean:
	-rm -f voronoi.o
	-rm -f voronoi
