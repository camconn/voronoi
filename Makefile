default: voronoi

voronoi.o: voronoi.c
	# NOTE: Do not use -O flags because they CAUSE SEGFAULTS
	gcc -c voronoi.c -o voronoi.o -std=gnu99 -lm -Wall

voronoi: voronoi.o
	# NOTE: Do not use -O flags because they CAUSE SEGFAULTS
	gcc voronoi.o -o voronoi -lm -std=gnu99 -Wall

clean:
	-rm -f voronoi.o
	-rm -f voronoi
