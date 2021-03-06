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

#define EUCLIDEAN 0
#define MANHATTAN 1
#define CHEBYSHEV 2

struct point {
	uint32_t x;
	uint32_t y;
};

double euclideanDist(uint32_t, uint32_t, uint32_t, uint32_t);
double manhattanDist(uint32_t, uint32_t, uint32_t, uint32_t);
double chebyshevDist(uint32_t, uint32_t, uint32_t, uint32_t);

uint32_t randXCoord(uint32_t);
uint32_t randYCoord(uint32_t);

void printHelp();
void updateCounter(uint64_t, uint64_t);
