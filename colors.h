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

#ifndef _COLORS_
#define _COLORS_

#define MAXTHEMES  64 // maximum number of themes
#define MAXCOLORS  20 // maximum colors per theme
#define MAXNAMELEN 64 // maximum length of theme name

#define CONFIG_ERROR 1

// Number to return if we'd rather print out a list of themes
#define LIST -10

struct RGB {
	int red;
	int green;
	int blue;
};

struct Theme {
	char* name;
	int   nameLen;
	int   numColors;
	int   colors[MAXCOLORS];
};

#endif /* _COLORS_ */
