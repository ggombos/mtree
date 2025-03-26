/*
 * contrib/mtree_gist/mtree_util.c
 */

#include "mtree_util.h"

#include "postgres.h"
// #include "fdlibm.h"

double string_distance(const char* a, const char* b)
{
	int lengthOfA = (int)strlen(a);
	int lengthOfB = (int)strlen(b);
	int x, y, lastDiagonal, oldDiagonal;
	double column[lengthOfA + 1];

	column[0] = 0;
	for (y = 1; y <= lengthOfA; ++y) {
		column[y] = y;
	}

	for (x = 1; x <= lengthOfB; ++x) {
		column[0] = x;
		for (y = 1, lastDiagonal = x - 1; y <= lengthOfA; ++y) {
			oldDiagonal = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastDiagonal + (a[y - 1] == b[x - 1] ? 0 : 1));
			lastDiagonal = oldDiagonal;
		}
	}

	return column[lengthOfA];
}

void init_distances(const int size, double* distances)
{
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			distances[i * size + j] = -1;
		}
	}
}

double overlap_area(double radiusOne, double radiusTwo, double distance)
{
	if (radiusOne == 0.0 || radiusTwo == 0.0 || distance == 0.0) {
		return 0;
	}

	double r1_square = radiusOne * radiusOne;
	double r2_square = radiusTwo * radiusTwo;
	double dst_square = distance * distance;

	double res;
	double acos_phi = (r1_square + dst_square - r2_square) / (2.0 * radiusOne * distance);
	double acos_theta = (r2_square + dst_square - r1_square) / (2.0 * radiusTwo * distance);

	double phi = (acos(acos_phi)) * 2.0;
	double theta = (acos(acos_theta)) * 2.0;
	double a1 = 0.5 * theta * r2_square - 0.5 * r2_square * sin(theta);
	double a2 = 0.5 * phi * r1_square - 0.5 * r1_square * sin(phi);

	res = a1 + a2;

	return res;
}

unsigned char get_array_length(const char* arrayString, const size_t arrayStringLength)
{
	char copyOfArrayString[arrayStringLength];
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
