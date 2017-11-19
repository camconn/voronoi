/*
 * voronoi - Generate random voronoi diagrams.
 * Copyright (C) 2015-2016 Cameron Conn    <cam {at} camconn {dot} cc>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
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


/*
 * Calculate the distance between two points using the standard Euclidean
 * distance formula
 */
double euclideanDist(uint32_t x, uint32_t y, uint32_t a, uint32_t b) {
	uint64_t horizDistance = llabs(((int64_t)x) - ((int64_t)a));
	uint64_t vertDistance = llabs(((int64_t)y) - ((int64_t)b));

	return sqrt(pow(horizDistance, 2) + pow(vertDistance, 2));
}

/*
 * Calculate the Manhattan distance between two points (x, y) and (a, b).
 */
double manhattanDist(uint32_t x, uint32_t y, uint32_t a, uint32_t b) {
	return llabs(((int64_t)(x)) - ((int64_t)(a))) + 
		llabs(((int64_t)(y)) - ((int64_t)(b)));
}

/*
 * Calculate the Chebyshev distance between two points (x, y) and (a, b).
 */
double chebyshevDist(uint32_t x, uint32_t y, uint32_t a, uint32_t b) {
	uint64_t horizDistance = llabs(((int64_t)x) - ((int64_t)a));
	uint64_t vertDistance = llabs(((int64_t)y) - ((int64_t)b));

	if (horizDistance > vertDistance) {
		return horizDistance;
	} else {
		return vertDistance;
	}
}

uint32_t randXCoord(uint32_t width) {
	return rand() % width;
}

uint32_t randYCoord(uint32_t height) {
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

void updateCounter(uint64_t current, uint64_t total) {
	float progress = (1.0 * current / total) * 100;
	printf("\rCurrent progress: %3.2f%%", progress);
}

int main(int argc, char *argv[]) {
	// Default values
	char *colorName = "standard";
	uint32_t numPoints = 500;
	uint32_t width = 500;
	uint32_t height = 500;
	uint32_t distType = EUCLIDEAN;
	char *filename = NULL;

	// Required for getopt()
	char c;
	extern char *optarg;
	/*
	 * There is a bug related to getopt here. It is resolved by using
	 * unistd.h instead of getopt.h (as is done with this program).
	 */
	while ((c = getopt(argc, argv, "c:n:w:t:d:p:h")) != 1) {
		switch (c) {
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
				printHelp();
				exit(0);
			case 'w':
				width = atoi(optarg);
				if (width == 0)
					fprintf(stderr, "Width must be greater than 0 pixels\n");
				break;
			case 't': 
				height = atoi(optarg);
				if (height == 0)
					fprintf(stderr, "Height must be greater than 0 pixels\n");
				break;
			case 'd':
				if (strlen(optarg) == 0) {
					fprintf(stderr, "Error: You must specify a distance type with the `-d` option.\n");
					exit(-1);
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
					exit(-1);
				}
				break;
			case 'n':
				if (strlen(optarg)) {
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
	memset(&(themes.themes), 0, sizeof(themes.themes));

	int ret = loadColors("colors.conf", &themes);
	if (ret == CONFIG_ERROR) {
		fprintf(stderr, "Could not load colors.conf!\n");
		exit(CONFIG_ERROR);
	}

	int16_t index = findTheme(&themes, colorName);

	if (index == -1) {
		fprintf(stderr, "Could not find theme \"%s\"\n", colorName);
		exit(-3);
	} else if (index == LIST) {
		printThemes(&themes);
		return 0;
	}

	struct Theme t;
	t = themes.themes[index];

	printf("Using theme %s\n", colorName);

	struct point *points = calloc(sizeof(struct point), numPoints);
	printf("Generating random points (%d)\n", numPoints);
	for (uint32_t i = 0; i < numPoints; i++) {
		points[i].x = randXCoord(width);
		points[i].y = randYCoord(height);
	}

	printf("Image dimensions: %d by %d\n", width, height);

	/* 
	 * For distanceFunc, a double return type is used because of the
	 * large magnitudes of the inputs and outputs.
	 */
	double (*distanceFunc)(uint32_t,uint32_t,uint32_t,uint32_t);
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
		exit(-3);
	}

	struct RGB *colorRGB = calloc(sizeof(struct RGB), t.numColors);
	for (uint8_t i = 0; i < t.numColors; i++) {
		hexToRGB(t.colors[i], &colorRGB[i]);
	}

	uint64_t totalPixels = width * height;
	uint64_t currentPixel = 0;

	FILE *file = fopen(filename, "w");

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);

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
	 *
	 * This is stated nowhere obvious in the libpng documentation. It
	 * infuriated me to know end until I discovered the *real* format
	 * myself.
	 */
	int row_len = sizeof(uint8_t) * 3 * width;

	for (uint32_t y = 0; y < height; y++) {
		png_byte *row = png_malloc(png_ptr, row_len);
		updateCounter(currentPixel, totalPixels);

		for (uint32_t x = 0; x < width; x++) {
			uint32_t closest = 0;
			double closestDist = width * height;
			// closestDist needs *some* initial value, so set it to the largest one possible
			for (uint32_t p = 0; p < numPoints; p++) {
				double dist = (*distanceFunc)(x, y, points[p].x, points[p].y);

				// TODO: Intrinsic?
				// TODO: likely() or unlikely()?
				if (dist < closestDist) {
					closest = p;
					closestDist = dist;
				}
			}

			struct RGB* color = &colorRGB[closest % t.numColors];
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

	// Clean up before ending.
	free(colorRGB);
	free(points);
	fclose(file);

	printf("Done! Output saved as \"%s\"\n", filename);
}
