/*
 * (c) Copyright 2015 Cameron Conn
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH     500
#define HEIGHT    500

#define NUMCOLORS 10
const long colors[10] = {
	0x000000, // black
	0xFFFFFF, // white
	0xFF0000, // red
	0x0000FF, // blue
	0x40E0D0, // turquoise
	0xFFD700, // gold
	0x800080, // purple
	0x00FF00, // lime green
	0xFA8072, // salmon
	0x8B0000  // darker red
};

struct point {
	int x;
	int y;
};

struct point points[10];

struct RGB {
	int red;
	int green;
	int blue;
};

// Calculate the distance between two points (x,y) and (a, b)
double distance(int x, int y, int a, int b) {
	int horizDistance = abs(x - a);
	int vertDistance = abs(y - b);

	double dist = sqrt(pow(horizDistance, 2) + 
			pow(vertDistance, 2));

	return dist;
}

int randXCoord() {
	return rand() % WIDTH;
}

int randYCoord() {
	return rand() % HEIGHT;
}

// interpret hex and write values to RGB struct
void hexToRGB(long hex, struct RGB *color) {
	color->red =   ((hex >> 16) & 0xFF);
	color->green = ((hex >> 8) & 0xFF);
	color->blue =  ((hex) & 0xFF);
}

void printColor(FILE *f, struct RGB *color) {
	fprintf(f, "%3d %3d %3d", color->red, color->green, color->blue);
}

int main(void) {
	printf("Starting program\n");

	printf("Making random points\n");
	// seed random with current time, otherwise we will get the SAME
	// IMAGES EVERYTIME!!!!!!?!?!!!!!
	time_t t;
	srand((unsigned)time(&t));

	// make rand points
	for (int i = 0; i < NUMCOLORS; i++) {
		points[i].x = randXCoord();
		points[i].y = randYCoord();
	}

	// points
	printf("Points:\n");
	for (int i = 0; i < NUMCOLORS; i++) {
		printf("Point %2d: (%3d, %3d)\n", i, points[i].x, points[i].y);
	}

	FILE *file;
	file = fopen("voronoi.ppm", "w");

	// write header
	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", WIDTH, HEIGHT);
	fprintf(file, "255\n");

	int p = 0;
	struct RGB *color = malloc(sizeof(struct RGB));

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (x != 0) {
				fprintf(file, "   ");
			}

			int closest;
			double closestDist = WIDTH * HEIGHT;
			for (p = 0; p < NUMCOLORS; p++) {
				//iterate through all pixels and find closest point
				double dist = distance(x, y, points[p].x, points[p].y);

				if (dist < closestDist) {
					closest = p;
					closestDist = dist;
				}
			}

			// printf("(%3d, %3d) # Closest point is (%3d, %3d)\n", x, y, points[closest].x, points[closest].y);


			//set to closest point's color value
			hexToRGB(colors[closest], color);
			printColor(file, color);
		}
		fprintf(file, "\n");
	}
	free(color);

	// save to disk
	fclose(file);

	printf("Done!\n");
}
