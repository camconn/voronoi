/*
 * voronoi - Generate random voronoi diagrams.
 * Copyright (C) 2016 Cameron Conn         <cam {at} camconn {dot} cc>
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

#pragma once

#include <stdint.h>
#include <stdio.h>

#define MAXTHEMES  (uint8_t)64 // maximum number of themes
#define MAXCOLORS  (uint8_t)20 // maximum colors per theme
#define MAXNAMELEN (uint8_t)64 // maximum length of theme name

#define CONFIG_ERROR 1

// Number to return if we'd rather print out a list of themes
#define LIST -10

struct RGB {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct Theme {
	uint32_t colors[MAXCOLORS];
	uint16_t numColors;
	char*    name;
};

typedef struct Pallet {
	uint16_t     numThemes;
	struct Theme themes[MAXTHEMES];
} Pallet;

int indexOf(char*, char);
void substr(char*, char*, uint8_t, uint8_t);

int loadColors(char* path, Pallet *themes);

void hexToRGB(uint32_t, struct RGB*);
void printColor(FILE*, struct RGB*);

void printThemes(Pallet*);
int16_t findTheme(Pallet*, char*);
