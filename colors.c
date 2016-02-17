/*
 * voronoi - Generate random voronoi diagrams
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "colors.h"

// interpret hex and write values to RGB struct
void hexToRGB(long hex, struct RGB *color) {
	color->red =   ((hex >> 16) & 0xFF);
	color->green = ((hex >> 8) & 0xFF);
	color->blue =  ((hex) & 0xFF);
}

void printColor(FILE *f, struct RGB *color) {
	fprintf(f, "%3d %3d %3d", color->red, color->green, color->blue);
}

typedef struct Pallet {
	int   numThemes;
	struct Theme themes[MAXTHEMES];
} Pallet;

int indexOf(char* term, char searchChar) {
	char* pos = strchr(term, searchChar);

	if (pos == NULL)
		return -1;
	else
		return (int)(pos - term); // voodoo array arithmetic
}

// Extract the key between a bracket `[]` pair
void substr(char* src, char* dst, int start, int len) {
	memcpy(dst, &src[start], len-1);
}

int loadColors(char* path, Pallet *themes) {
	FILE *configFile;

	configFile = fopen(path, "r");

	struct Theme *current;

	char buf[256];
	while(fgets(buf, sizeof(buf), configFile)) {
		// skip blank lines
		if (strlen(buf) == 1) {
			continue;
		}	
		
		current = &(themes->themes[themes->numThemes-1]);

		// printf("buf: %s\n", buf);

		// TODO: Make this more robust
		if (buf[0] == '[') { // theme name
			int i = indexOf(buf, ']');


			if (i == -1) {
				goto fail;
			} else {
				struct Theme newTheme;
				newTheme.nameLen = 0;
				newTheme.numColors = 0;
				newTheme.name = "";
				themes->themes[themes->numThemes] = newTheme;

				char* name = (char*)malloc(MAXNAMELEN+1);
				substr(buf, name, 1, i);

				themes->themes[themes->numThemes].name = name;
				themes->themes[themes->numThemes].nameLen = i-1;

				for (int j = 0; j < MAXCOLORS; j++)
					themes->themes[themes->numThemes].colors[j] = 0;

				themes->numThemes++;
			}

		} else { //must be a hex value!
			int i = indexOf(buf, '\n');

			if (i == -1) {
				fprintf(stderr, "How does this crap happen?\n");
				continue;
			}


			// append hex to index
			// char hex[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
			char hex[] = {0, 0, 0, 0, 0, 0, '\0'};
			// substr(buf, hex[2], 0, strlen(buf));
			hex[0] = buf[0];
			hex[1] = buf[1];
			hex[2] = buf[2];
			hex[3] = buf[3];
			hex[4] = buf[4];
			hex[5] = buf[5];
			
			unsigned int value = 0;
			sscanf(hex, "%x", &value);

			current->colors[current->numColors] = value;
			current->numColors++;
		}

		memset(buf, 0, sizeof(buf));
	}

	return 0;
fail:
	return CONFIG_ERROR;

}

void printThemes(Pallet *themes) {
	printf("Available Themes (%d):\n", themes->numThemes);

	for (int i = 0; i < themes->numThemes; i++) {
		printf("%s\n", themes->themes[i].name);
	}
}


// find the index of a theme by using its name
int findTheme(Pallet *themes, char* name) {
	// convert string to lowercase
	if (strcmp(name, "list") == 0) {
		return LIST;
	}

	for (int i = 0; i < themes->numThemes; i++) {
		if (strcmp(name, (themes->themes[i]).name) == 0) {
			return i;
		}
	}
	return -1;
}
