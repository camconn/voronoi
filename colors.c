#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const long standard[11] = {
	0x000000, // black
	0xFFFFFF, // white
	0x708090, // slate gray
	0x0000FF, // blue
	0x40E0D0, // turquoise
	0xFFD700, // gold
	0x800080, // purple
	0x00FF00, // lime green
	0xFA8072, // salmon
	0x90EE90, // light green
	0x87421F  // brown
};

const long rainbow[5] = {
	0x1B00AF, // blue
	0x1FC3F7, // aqua
	0xC8ED09, // yellow-ish
	0xFA1C09, // orange
	0xD81014 // red
};

const long warm[5] = {
	0x1F1C0B, // gray-brown
	0x80362F, // brown-ish
	0xF08C86, // pink-ish
	0xDBD0D2, // off-white
	0xF58758, // peach
};

const long blues[5] = {
	0x20D9DB, // cyan
	0x23BDD4, // light blue
	0x1A6FE0, // blue
	0x231E60, // dark blue
	0x11342F, // dark green
};

const long autumn[5] = {
	0xF3B805, // yellow
	0xF0A103, // gold
	0xD96801, // orange
	0x311B00, // brown
	0xBE2802, // dark orange
};

enum {
	DEFAULT = 0,
	AUTUMN,
	BLUES,
	RAINBOW,
	WARM
};


void copyArray(int n, const long src[], long dst[]) {
	for (int i = 0; i < n; i++) {
		dst[i] = src[i];
	}
}

void writeColor(int num, long colors[]) {
	switch (num) {
		case DEFAULT:
			copyArray(11, standard, colors);
			break;
		case AUTUMN:
			copyArray(5, autumn, colors);
			break;
		case BLUES:
			copyArray(5, blues, colors);
			break;
		case RAINBOW:
			copyArray(5, rainbow, colors);
			break;
		case WARM:
			copyArray(5, warm, colors);
			break;
	}
}

int palletColors(int colorNum) {
	switch (colorNum) {
		case DEFAULT:
			return 11;
		case AUTUMN:
		case BLUES:
		case RAINBOW:
		case WARM:
			return 5;
	}
	return -1;
}

// determine the size of the color array (in bytes)
int colorSize(int colorNum) {
	switch (colorNum) {
		case DEFAULT:
			return sizeof(standard);
		case AUTUMN:
		case BLUES:
		case RAINBOW:
		case WARM:
			return sizeof(autumn);
	}
	return -1;
}

// get the enum of the color needed
int colorNum(char* color) {
	if (strcmp(color, "autumn") == 0) {
		return AUTUMN;
	} else if (strcmp(color, "rainbow") == 0) {
		return RAINBOW;
	} else if (strcmp(color, "blues") == 0) {
		return BLUES;
	} else if (strcmp(color, "warm") == 0) {
		return WARM;
	} else if (strcmp(color, "standard") == 0 ||
			strlen(color) == 0 ) {
		return DEFAULT;
	} else if (strcmp(color, "list") == 0) {
		return -2;
	} else {
		return -1;
	}

}

void printColors() {
	printf("\n");
	printf("Color theme list\n");
	printf("==============\n");
	printf("Standard\n");
	printf("Autumn\n");
	printf("Blues\n");
	printf("Rainbow\n");
	printf("Warm\n");
}

