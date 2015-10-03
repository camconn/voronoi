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

#include "colors.c"

#define WIDTH     500
#define HEIGHT    500

struct point {
	int x;
	int y;
};

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

	time_t t;
	srand((unsigned)time(&t));
	int numColors = sizeof(rainbow) / sizeof(rainbow[0]);
	int colors[numColors];

	// copy values over
	for (int i = 0; i < numColors; i++) {
		colors[i] = rainbow[i];
	}

	int numPoints = 500;
	struct point points[numPoints];

	printf("Making random points\n");
	// seed random with current time, otherwise we will get the SAME
	// IMAGES EVERYTIME!!!!!!?!?!!!!!
	for (int i = 0; i < numPoints; i++) {
		points[i].x = randXCoord();
		points[i].y = randYCoord();
	}

	// points
	printf("Points:\n");
	for (int i = 0; i < numPoints; i++) {
		printf("Point %2d: (%3d, %3d)\n", i+1, points[i].x, points[i].y);
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
			for (p = 0; p < numPoints; p++) {
				//iterate through all pixels and find closest point
				double dist = distance(x, y, points[p].x, points[p].y);

				if (dist < closestDist) {
					closest = p;
					closestDist = dist;
				}
			}

			// printf("(%3d, %3d) # Closest point is (%3d, %3d)\n", x, y, points[closest].x, points[closest].y);


			//set to closest point's color value
			hexToRGB(colors[closest % numColors], color);
			printColor(file, color);
		}
		fprintf(file, "\n");
	}
	free(color);

	// save to disk
	fclose(file);

	printf("Done!\n");
}
