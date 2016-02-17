/*
 * voronoi - Generate random voronoi diagrams.
 * Copyright (C) 2016 Cameron Conn

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
#include <stdint.h>

#include <png.h>

#include "voronoi.h"
#include "colors.h"
#include "colors.c"


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

// Calculate the distance between two points (x, y) and (a, b)
// using the Chebyshev distance.
// Returns a double to keep compatibility with euclidean dist
double chebyshevDist(int x, int y, int a, int b) {
	int distHoriz = abs(x - a);
	int distVert  = abs(y - b);

	if (distHoriz > distVert) {
		return distHoriz;
	} else {
		return distVert;
	}
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
	printf("-n PATH      Where to save the generated image (default: voronoi.png)\n");
	printf("-c COLOR     Color theme to use. Using `-c LIST` will list the\n");
	printf("             themes available\n");
	printf("-d DISTTYPE  Type of distance to use. Available distance types:\n");
	printf("             euclidean (default), manhattan, chebyshev\n");
	printf("-p POINTS    Number of points to use\n");
	printf("-t NUM       Make an image NUM pixels tall (default 500)\n");
	printf("-w NUM       Make an image NUM pixels wide (default 500)\n");
	printf("-h           Prints this message\n");
}

void updateCounter(int current, int total) {
	double progress = (1.0 * current / total) * 100;
	printf("\rCurrent progress: %3.2f%%", progress);
}

int main(int argc, char *argv[]) {
	char *colorName = "standard";
	char *filename = NULL;
	extern char *optarg;
	char c;
	int distType = EUCLIDEAN;

	// Default values
	int numPoints = 500;
	int width = 500;
	int height = 500;

	// Workaround
	int len;

	// For some strange reason the first letter of the control string of
	// getopt (argument three) is ignored...
	// ergo, we use <unistd.h>
	while ((c = getopt(argc, argv, "c:n:w:t:d:p:h")) != 1) {
		switch (c) {
			// stuff
			case 'p':
				// printf("Got points\n");
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
				} else if (strcmp(optarg, "chebyshev") == 0) {
					distType = CHEBYSHEV;
				} else {
					fprintf(stderr, "Unknown distance type: %s\n", optarg);
					printHelp();
					return -1;
				}
				break;
			case 'n':
				len = (strlen(optarg));
				if (len == 0) {
					printf("You need to specify a filename when using `-n`. Using default name instead.\n");
					filename = "voronoi.png";
				} else {
					filename = optarg;
				}
				break;
			case '?':
				//printf("how?\n");
				// ignore unknown flags
				continue;
			default:
				goto tail;
		}
	}
tail:
	// We seed the rng because if we don't then we'll get the same image
	// EVERY FREAKING TIME!!!!?!!!
	srand((unsigned)time(NULL));

	// check if filename is the default one
	if (filename == NULL)
		filename = "voronoi.png";

	Pallet themes;
	struct Theme t;
	loadColors("colors.conf", &themes);

	int index = findTheme(&themes, colorName);

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
	printf("Generating random points (%d)\n", numPoints);
	for (int i = 0; i < numPoints; i++) {
		points[i].x = randXCoord(width);
		points[i].y = randYCoord(height);
	}

	printf("Image dimensions: %d by %d\n", width, height);

	// We use a function pointer to reuse the distance calculating code
	double (*distanceFunc)(int,int,int,int);
	if (distType == EUCLIDEAN) {
		distanceFunc = &euclideanDist;
		printf("Using Euclidean distance\n");
	} else if (distType == MANHATTAN) {
		distanceFunc = &manhattanDist;
		printf("Using Manhattan distance\n");
	} else if (distType == CHEBYSHEV) {
		distanceFunc = &chebyshevDist;
		printf("Using Chebyshev distance\n");
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

	FILE *file = fopen(filename, "w");

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);

# if 0
	if (setjmp(png_jmpbuf(png_ptr)))
		exit(-1);
#endif

	png_init_io(png_ptr, file);

	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/*
	 * row_len is how long each row is for the png bytes
	 * `sizeof(uint8_t) * 3`  is the size of each RGB pixels's data,
	 * meaning that there is one byte (or uint8_t) for each
	 * value of Red, Blue, and Green. This adds up to 24 bits (3 bytes)
	 * per pixel.
	 */
	int row_len = sizeof(uint8_t) * 3 * width;

	for (int y = 0; y < height; y++) {
		png_byte *row = png_malloc(png_ptr, row_len);
		updateCounter(currentPixel, totalPixels);

		for (int x = 0; x < width; x++) {
			int closest = 0;
			double closestDist = width * height; // We need an initial value, so set it to the maximum possible
			for (p = 0; p < numPoints; p++) {
				double dist = (*distanceFunc)(x, y, points[p].x, points[p].y);

				// TODO: Intrinsic?
				// TODO: likely() or unlikely()?
				if (dist < closestDist) {
					closest = p;
					closestDist = dist;
				}
			}

			struct RGB* color = colorRGB[closest % t.numColors];
			*row++ = ((uint8_t)(color->red));
			*row++ = ((uint8_t)(color->green));
			*row++ = ((uint8_t)(color->blue));
		}

		currentPixel += width;

		row -= row_len;
		// put that pointer right back where it came from (so help me!)

		png_write_row(png_ptr, row);
	}

	// call this one more time just we we get that sweet 100% progress 
	updateCounter(currentPixel, totalPixels);
	printf("\n");

	png_write_end(png_ptr, info_ptr);

	// sync to disk
	fclose(file);

	printf("Done! Output saved as \"%s\"\n", filename);
}
