default: voronoi

voronoi.o: voronoi.c
	gcc -c voronoi.c -o voronoi.o -std=gnu99 -lm -Wall

voronoi: voronoi.o
	gcc voronoi.o -o voronoi -lm -std=gnu99 -Wall

clean:
	-rm -f voronoi.o
	-rm -f voronoi
