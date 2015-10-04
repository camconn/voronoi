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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXTHEMES  64 // maximum number of themes
#define MAXCOLORS  20 // maximum colors per theme
#define MAXNAMELEN 64 // maximum length of theme name

#define CONFIG_ERROR 1

typedef struct Theme {
	char* name;
	int   nameLength;
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
				printf("Now have key name \"%s\"\n", name);

				themes->themes[themes->numThemes].name = name;
				themes->themes[themes->numThemes].nameLength = i-1;

				themes->numThemes++;
			}

		} else { //must be a hex value!
			int i = indexOf(buf, '\n');

			if (i == -1) {
				printf("How does this crap happen?\n");
				continue;
			}

			Theme *current = &(themes->themes[themes->numThemes-1]);

			// append hex to index
			char* hex = (char*)malloc(9);
			substr(buf, &hex[2], 0, strlen(buf));
			// hack to append "0x" to beginning of hex for proper parsing
			hex[0] = '0';
			hex[1] = 'x';
			
			printf("Hex \"%s\"\n", hex);

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
void printColors() {
	// TODO: Make this dynamic!
	printf("\n");
	printf("Color theme list\n");
	printf("==============\n");
	printf("Standard\n");
	printf("Autumn\n");
	printf("Blues\n");
	printf("Rainbow\n");
	printf("Theif\n");
	printf("Toxic\n");
	printf("Warm\n");
}

