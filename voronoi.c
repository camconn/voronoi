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
#include <getopt.h>
#include <string.h>

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

void printHelp() {
	printf("Usage: voronoi [OPTION]\n");
	printf("Create voronoi Diagrams\n");
	printf("\n");
	printf("-p POINTS    Number of points to use\n");
	printf("-c COLOR     Color theme to use\n");
	printf("-l           Lists available color themes\n");
	printf("-h           Prints this message\n");
}


int main(int argc, char **argv) {
	char *colorName = NULL;
	extern char *optarg;
	int numPoints = 500;
	int c;

	while ((c = getopt(argc, argv, "hlp:c:")) != 1) {
		switch (c) {
			case 'p':
				//printf("points\n");
				numPoints = atoi(optarg);
				printf("Got points\n");
				if (numPoints == 0) {
					fprintf(stderr, "Invalid number of points: %s\n", optarg);
					exit(-2);
				}
				break;
			case 'c':
				// printf("color\n");
				colorName = malloc(sizeof(optarg));
				strcpy(colorName, optarg);
				break;
			case 'h':
				// printf("help");
				printHelp();
				return 0;
			case 'l':
				// printf("list");
				printColors();
				return 0;
			default:
				goto tail;
		}
	}
tail:
	// We seed the rng becaues if we don't then we'll get the same image
	// EVERY FREAKING TIME!!!!?!!!
	srand((unsigned)time(NULL));

	if (colorName == NULL) {
		printf("No theme specified, using standard theme.\n");
		colorName = "standard";
	}
	int colNum = colorNum(colorName);

	if (colNum == -1) {
		fprintf(stderr, "Invalid color name: %s\n", colorName);
		exit(-1);
	}


	printf("Color number: %d\n", colNum);

	// get size to malloc()
	long *colors;
	colors = malloc(colorSize(colNum));

	writeColor(colNum, colors);

	int numColors = palletColors(colNum);
	writeColor(colNum, colors);

	printf("Using %s pallet\n", colorName);

	struct point points[numPoints];

	printf("Making random points (%d)\n", numPoints);
	// seed random with current time, otherwise we will get the SAME
	// IMAGES EVERYTIME!!!!!!?!?!!!!!
	for (int i = 0; i < numPoints; i++) {
		points[i].x = randXCoord();
		points[i].y = randYCoord();
	}

	// points

	FILE *file;
	file = fopen("voronoi.ppm", "w");

	// write header
	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", WIDTH, HEIGHT);
	fprintf(file, "255\n");

	int p = 0;
	struct RGB *color = malloc(sizeof(struct RGB));

	printf("Creating image...");

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
