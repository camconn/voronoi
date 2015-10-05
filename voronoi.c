/*
 * voronoi - Generate random voronoi diagrams
 * Copyright (C) 2015 Cameron Conn

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "colors.c"

#define EUCLIDEAN 0
#define MANHATTAN 1

struct point {
	int x;
	int y;
};

// Calculate the distance between two points (x,y) and (a, b)
// uses the standard euclidean distance formula
double euclideanDist(int x, int y, int a, int b) {
	int horizDistance = abs(x - a);
	int vertDistance = abs(y - b);

	double dist = sqrt(pow(horizDistance, 2) + 
			pow(vertDistance, 2));

	return dist;
}

// Calculate the distance between two points (x,y) and (a,b)
// using the Manhattan distance
// We return a double type to keep compatibility with
// euclideanDist
double manhattanDist(int x, int y, int a, int b) {
	return abs(x - a) + abs(y - b);
}

int randXCoord(int width) {
	return rand() % width;
}

int randYCoord(int height) {
	return rand() % height;
}

void printHelp() {
	printf("Usage: voronoi [OPTIONS]\n");
	printf("Create randomly generated voronoi Diagrams\n");
	printf("\n");
	printf("Available options:\n");
	printf("-c COLOR     Color theme to use. Using `-c LIST` will list the\n");
	printf("             themes available\n");
	printf("-d DISTTYPE  Type of distance to use. Available distance types:\n");
	printf("             euclidean (default), manhattan\n");
	printf("-p POINTS    Number of points to use\n");
	printf("-t NUM       Make an image NUM pixels tall (default 500)\n");
	printf("-w NUM       Make an image NUM pixels wide (default 500)\n");
	printf("-h           Prints this message\n");
}

void updateCounter(int current, int total) {
	double progress = (1.0 * current / total) * 100;
	printf("\rCurrent progress: %3.2f%%", progress);
}

int main(int argc, char **argv) {
	char *colorName = "standard";
	extern char *optarg;
	int numPoints = 500;
	int c;
	int distType = EUCLIDEAN;

	int width =  500;
	int height = 500;

	// For some strange reason the first letter of the control string of
	// getopt (argument three) is ignored...
	// ergo, we use <unistd.h>
	while ((c = getopt(argc, argv, "c:w:t:d:p:h")) != 1) {
		switch (c) {
			// stuff
			case 'p':
				printf("Got points\n");
				numPoints = atoi(optarg);
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
			case 'd':
				if (strlen(optarg) == 0) {
					fprintf(stderr, "Error: You must specify a distance type with the `-d` option.\n");
					return -1;
				}

				if (strcmp(optarg, "euclidean") == 0) {
					distType = EUCLIDEAN;
				} else if (strcmp(optarg, "manhattan") == 0) {
					distType = MANHATTAN;
				} else {
					fprintf(stderr, "Unknown distance type: %s\n", optarg);
					printHelp();
					return -1;
				}
			case '?':
				//printf("wut?\n");
				continue;
			default:
				goto tail;
		}
	}
tail:
	// We seed the rng becaues if we don't then we'll get the same image
	// EVERY FREAKING TIME!!!!?!!!
	srand((unsigned)time(NULL));

	Pallet themes;
	Theme t;
	loadColors("colors.conf", &themes);

	int index = findTheme(&themes, &t, colorName);

	if (index == -1) {
		fprintf(stderr, "Could not find theme \"%s\"\n", colorName);
		return -3;
	} else if (index == LIST) {
		printThemes(&themes);
		return 0;
	}

	t = themes.themes[index];

	printf("Loaded theme %s\n", colorName);
	
	struct point points[numPoints];
	printf("Making random points (%d)\n", numPoints);
	// seed random with current time, otherwise we will get the SAME
	// IMAGES EVERYTIME!!!!!!?!?!!!!!
	for (int i = 0; i < numPoints; i++) {
		points[i].x = randXCoord(width);
		points[i].y = randYCoord(height);
	}

	printf("Image dimensions: %d by %d\n", width, height);
	printf("Creating image...\n");

	// We use a function pointer to reuse the distance calcuating code
	double (*distanceFunc)(int,int,int,int);
	if (distType == EUCLIDEAN) {
		distanceFunc = &euclideanDist;
		printf("Using Euclidean distance\n");
	} else if (distType == MANHATTAN) {
		distanceFunc = &manhattanDist;
		printf("Using Manhattan distance\n");
	} else {
		printf("Unknown distance type... Exiting\n");
		return -3;
	}

	struct RGB *colorRGB[t.numColors];

	for (int i = 0; i < t.numColors; i++) {
		colorRGB[i] = (struct RGB*)malloc(sizeof(struct RGB));
		hexToRGB(t.colors[i], colorRGB[i]);
	}

	int p = 0;
	int totalPixels = width * height;
	int currentPixel = 0;

	// allocating image in memory
	struct RGB *pixels[width];

	// open file
	FILE *file;
	file = fopen("voronoi.ppm", "w");

	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", width, height);
	fprintf(file, "255\n");

	for (int y = 0; y < height; y++) {
		updateCounter(currentPixel, totalPixels);
		for (int x = 0; x < width; x++) {
			currentPixel++;

			int closest;
			double closestDist = width * height; // We need an initial value, so set it to the maximum possible
			for (p = 0; p < numPoints; p++) {
				double dist = (*distanceFunc)(x, y, points[p].x, points[p].y);

				if (dist < closestDist) {
					closest = p;
					closestDist = dist;
				}
			}

			// ignore warning about closest being uninitialized, because
			// logically that cannot happen
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
			pixels[x] = colorRGB[closest % t.numColors];
#pragma GCC diagnostic pop

		}

		// write pixels to disk one line at a time
		for (int x = 0; x < width; x++) {
			fprintf(file, "%3d %3d %3d   ", pixels[x]->red, pixels[x]->green, pixels[x]->blue);
		}

		fprintf(file, "\n");
	}
	// call this one more time just we we get that sweet 100% progress 
	updateCounter(currentPixel, totalPixels);
	printf("\n");

	// sync to disk
	fclose(file);

	printf("Done!\n");
}

