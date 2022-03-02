/*
 * contrib/mtree_gist/mtree_util.c
 */

#include "mtree_util.h"

int string_distance(char* a, char* b) {
	int lengthOfA = (int)strlen(a);
	int lengthOfB = (int)strlen(b);
	int x, y, lastDiagonal, oldDiagonal;
	int column[lengthOfA + 1];

	column[0] = 0;
	for (y = 1; y <= lengthOfA; ++y) {
		column[y] = y;
	}

	for (x = 1; x <= lengthOfB; ++x) {
		column[0] = x;
		for (y = 1, lastDiagonal = x - 1; y <= lengthOfA; ++y) {
			oldDiagonal = column[y];
			column[y] = MIN3(
				column[y] + 1,
				column[y - 1] + 1,
				lastDiagonal + (a[y - 1] == b[x - 1] ? 0 : 1)
			);
			lastDiagonal = oldDiagonal;
		}
	}

	return column[lengthOfA];
}

void init_distances(int size, int* distances) { /* TODO: int distances[size][size] -> int* distances */
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			distances[i * size + j] = -1;
		}
	}
}

double overlap_area(int radiusOne, int radiusTwo, int distance) {
	if (radiusOne == 0 || radiusTwo == 0 || distance == 0) {
		return 0;
	}

	int radiusOneSquare = radiusOne * radiusOne;
	int radiusTwoSquare = radiusTwo * radiusTwo;
	int distanceSquare = distance * distance;

	double phi = (acos((radiusOneSquare + distanceSquare - radiusTwoSquare) / (2 * radiusOne * distance))) * 2;
	double theta = (acos((radiusTwoSquare + distanceSquare - radiusOneSquare) / (2 * radiusTwo * distance))) * 2;
	double a1 = 0.5 * theta * radiusTwoSquare - 0.5 * radiusTwoSquare * sin(theta);
	double a2 = 0.5 * phi * radiusOneSquare - 0.5 * radiusOneSquare * sin(phi);

	return a1 + a2;
}

unsigned char get_array_length(const char* arrayString) {
	char copyOfArrayString[strlen(arrayString)];
	strcpy(copyOfArrayString, arrayString);

	unsigned char numberOfCommas = 0;
	for (unsigned short int i = 0; copyOfArrayString[i]; ++i) {
		if (copyOfArrayString[i] == ',') {
			numberOfCommas += 1;
		}
	}

	if (numberOfCommas != 0) {
		numberOfCommas += 1;
	}

	return numberOfCommas;
}
