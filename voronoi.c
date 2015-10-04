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

int randXCoord(int width) {
	return rand() % width;
}

int randYCoord(int height) {
	return rand() % height;
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
	printf("-c COLOR     Color theme to use. Using `-c LIST` will list the\n");
	printf("             themes available\n");
	printf("-w NUM       Make an image NUM pixels wide (default 500)\n");
	printf("-t NUM       Make an image NUM pixels tall (default 500)\n");
	printf("-h           Prints this message\n");
}

void updateCounter(int current, int total) {
	double progress = (1.0 * current / total) * 100;
	printf("\rCurrent progress: %3.2f%%", progress);
}


int main(int argc, char **argv) {
	char *colorName = NULL;
	extern char *optarg;
	int numPoints = 500;
	int c;

	int width =  500;
	int height = 500;

	while ((c = getopt(argc, argv, "hlp:c:w:t:")) != 1) {
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
				colorName = malloc(sizeof(optarg));
				strcpy(colorName, optarg);
				break;
			case 'h':
				// printf("help");
				printHelp();
				return 0;
			case 'w':
				width = atoi(optarg);
				if (width <= 0)
					fprintf(stderr, "Width must be greater than 0 pixels\n");
				break;
			case 't': 
				height = atoi(optarg);
				if (height <= 0) 
					fprintf(stderr, "Height must be greater than 0 pixels\n");
				break;
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
		fprintf(stderr, "Invalid theme name: %s\n", colorName);
		exit(-1);
	} else if (colNum == -2) {
		printColors();
		return 0;
	}

	printf("Color number: %d\n", colNum);

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
		points[i].x = randXCoord(width);
		points[i].y = randYCoord(height);
	}

	int totalPixels = width * height;
	int currentPixel = 0;

	FILE *file;
	file = fopen("voronoi.ppm", "w");

	// write header
	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", width, height);
	fprintf(file, "255\n");

	int p = 0;
	struct RGB *color = malloc(sizeof(struct RGB));

	printf("Image dimensions: %d by %d\n", width, height);
	printf("Creating image...\n");

	for (int y = 0; y < height; y++) {
		updateCounter(currentPixel, totalPixels);
		for (int x = 0; x < width; x++) {
			currentPixel++;
			if (x != 0) {
				fprintf(file, "   ");
			}

			int closest;
			double closestDist = width * height;
			for (p = 0; p < numPoints; p++) {
				double dist = distance(x, y, points[p].x, points[p].y);

				if (dist < closestDist) {
					closest = p;
					closestDist = dist;
				}
			}

			//set to closest point's color value
			hexToRGB(colors[closest % numColors], color);
			printColor(file, color);
		}
		fprintf(file, "\n");
	}
	
	// call this one more time just we we get that sweet 100% progress 
	updateCounter(currentPixel, totalPixels);

	printf("\n");
	free(color);

	// save to disk
	fclose(file);

	printf("Done!\n");
}
