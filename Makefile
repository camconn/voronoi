default: voronoi

voronoi.o: voronoi.c
	gcc -c voronoi.c -o voronoi.o -std=gnu99 -lm -lpng -Wall -O3 -march=native

voronoi: voronoi.o
	gcc voronoi.o -o voronoi -lm -lpng -std=gnu99 -Wall -O3 -march=native

clean:
	-rm -f voronoi.o
	-rm -f voronoi
