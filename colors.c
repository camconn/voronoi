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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define MAXTHEMES  64 // maximum number of themes
#define MAXCOLORS  20 // maximum colors per theme
#define MAXNAMELEN 64 // maximum length of theme name

#define CONFIG_ERROR 1

struct RGB {
	int red;
	int green;
	int blue;
};

// interpret hex and write values to RGB struct
void hexToRGB(long hex, struct RGB *color) {
	color->red =   ((hex >> 16) & 0xFF);
	color->green = ((hex >> 8) & 0xFF);
	color->blue =  ((hex) & 0xFF);
}

void printColor(FILE *f, struct RGB *color) {
	fprintf(f, "%3d %3d %3d", color->red, color->green, color->blue);
}

typedef struct Theme {
	char* name;
	int   nameLen;
	int   numColors;
	int   colors[MAXCOLORS];
} Theme;

typedef struct Pallet {
	int   numThemes;
	Theme themes[MAXTHEMES];
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

void clean(char arr[]) {
	for (int i = 0; i < sizeof(arr); i++) {
		arr[i] = 0;
	}
}

int loadColors(char* path, Pallet* themes) {
	FILE *configFile;

	configFile = fopen(path, "r");

	char buf[256];
	while(fgets(buf, sizeof(buf), configFile)) {
		// skip blank lines
		if (strlen(buf) == 1) {
			continue;
		}	

		// TODO: Make this more robust
		if (buf[0] == '[') { // theme name
			int i = indexOf(buf, ']');

			Theme t;

			if (i == -1) {
				goto fail;
			} else {
				themes->themes[themes->numThemes] = t;

				char* name = (char*)malloc(MAXNAMELEN+1);
				substr(buf, name, 1, i);

				themes->themes[themes->numThemes].name = name;
				themes->themes[themes->numThemes].nameLen = i-1;

				themes->numThemes++;
			}

		} else { //must be a hex value!
			int i = indexOf(buf, '\n');

			if (i == -1) {
				fprintf(stderr, "How does this crap happen?\n");
				continue;
			}

			Theme *current = &(themes->themes[themes->numThemes-1]);

			// append hex to index
			char* hex = (char*)malloc(9);
			substr(buf, &hex[2], 0, strlen(buf));
			// hack to append "0x" to beginning of hex for proper parsing
			hex[0] = '0';
			hex[1] = 'x';
			
			long value = strtol(hex, NULL, 0);

			// printf("got hex value \"%x\"\n", (unsigned)value);

			current->colors[current->numColors] = value;
			current->numColors++;
		}

		memset(buf, 0, sizeof(buf));
	}



	return 0;
fail:
	return CONFIG_ERROR;

}

void copyArray(int n, const long src[], long dst[]) {
	for (int i = 0; i < n; i++) {
		dst[i] = src[i];
	}
}

void printThemes(Pallet *themes) {
	printf("Color theme list (%d themes available)\n", themes->numThemes);
	printf("==============\n");

	for (int i = 0; i < themes->numThemes; i++) {
		printf("%s\n", themes->themes[i].name);
	}
}

// Number to return if we'd rather print out a list of themes
#define LIST -10

// find the index of a theme by using its name
int findTheme(Pallet *themes, Theme *t, char* name) {
	// convert string to lowercase
	if (strcmp(name, "list") == 0) {
		return LIST;
	}

	for (int i = 0; i < themes->numThemes; i++) {
		// printf("%d\n", t->nameLen);
		if (strcmp(name, (themes->themes[i]).name) == 0) {
			t = &themes->themes[i];
			return i;
		}
	}
	return -1;
}
