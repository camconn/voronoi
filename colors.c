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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "colors.h"

// interpret hex and write values to RGB struct
void hexToRGB(uint32_t hex, struct RGB *color) {
	color->red =   ((hex >> 16) & 0xFF);
	color->green = ((hex >> 8) & 0xFF);
	color->blue =  ((hex) & 0xFF);
}

void printColor(FILE *f, struct RGB *color) {
	fprintf(f, "%3d %3d %3d", color->red, color->green, color->blue);
}

int indexOf(char* term, char searchChar) {
	char* pos = strchr(term, searchChar);

	if (pos == NULL)
		return -1;
	else
		return (int)(pos - term); // voodoo array arithmetic
}

// Extract the key between a bracket `[]` pair
void substr(char* src, char* dst, uint8_t start, uint8_t len) {
	memcpy(dst, &src[start], len-1);
}

int loadColors(char* path, Pallet *themes) {
	FILE *configFile;

	configFile = fopen(path, "r");
	if (configFile == NULL) {
		fprintf(stderr, "Could not find colors.conf.\n");
		exit(-4);
	}

	struct Theme *current;
	themes->numThemes = 0;

	char* line = NULL;
	size_t len = 128;
	ssize_t read;

	while ((read = getline(&line, &len, configFile)) > 0) {
		// skip blank and lines that are too short
		// TODO: Make this more robust
		if (line[0] == '[') { // theme name
			// Clear things out
			current = &(themes->themes[themes->numThemes]);
			memset(current, 0, sizeof(struct Theme));
			memset(current->colors, 0, sizeof(uint32_t[MAXCOLORS]));

			int i = indexOf(line, ']');

			if (i == -1) {
				goto fail;
			} else {
				char* name = (char*)malloc(MAXNAMELEN+1);
				substr(line, name, 1, i);

				current->name = name;

				for (uint8_t j = 0; j < MAXCOLORS; j++)
					current->colors[j] = 0;
			}
		} else if (strlen(line) > 1) { //must be a hex value!
			if (indexOf(line, '\n') == -1) {
				fprintf(stderr, "How does this crap happen?\n");
				//continue;
			} else if (indexOf(line, '#') == 0) {
				continue;
			}

			char hex[8];
			strcpy(hex, line);
			hex[6] = 0;

			uint32_t value = (uint32_t)strtol(hex, NULL, 16);

			current->colors[current->numColors] = value;
			current->numColors++;
		} else {
			themes->numThemes++;
		}
	}

	fclose(configFile);
	return 0;
fail:
	fclose(configFile);
	return CONFIG_ERROR;

}

void printThemes(Pallet *themes) {
	printf("Available Themes (%d):\n", themes->numThemes);

	for (uint16_t i = 0; i < themes->numThemes; i++) {
		printf("%s\n", themes->themes[i].name);
	}
}


// find the index of a theme by using its name
int16_t findTheme(Pallet *themes, char* name) {
	// convert string to lowercase
	if (strcasecmp(name, "list") == 0) {
		return LIST;
	}

	for (uint16_t i = 0; i < themes->numThemes; i++) {
		if (strcmp(name, (themes->themes[i]).name) == 0) {
			return i;
		}
	}
	return -1;
}
