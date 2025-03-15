/*
 * contrib/mtree_gist/mtree_util.c
 */

#include "mtree_util.h"
#include "postgres.h"
// #include "fdlibm.h"

int string_distance(const char* a, const char* b)
{
	int lengthOfA = (int) strlen(a);
	int lengthOfB = (int) strlen(b);
	int x, y, lastDiagonal, oldDiagonal;
	int column[lengthOfA + 1];

	column[0] = 0;
	for (y = 1; y <= lengthOfA; ++y)
	{
		column[y] = y;
	}

	for (x = 1; x <= lengthOfB; ++x)
	{
		column[0] = x;
		for (y = 1, lastDiagonal = x - 1; y <= lengthOfA; ++y)
		{
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

void init_distances_long_long(const int size, long long* distances)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			distances[i * size + j] = -1;
		}
	}
}

void init_distances_float(const int size, float* distances)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			distances[i * size + j] = -1.0;
		}
	}
}

void init_distances_int(const int size, int* distances)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			distances[i * size + j] = -1;
		}
	}
}

double overlap_area(const long long radiusOne, const long long radiusTwo, const long long distance)
{
	if (radiusOne == 0 || radiusTwo == 0 || distance == 0)
	{
		return 0;
	}

	if(radiusOne > distance + radiusTwo){
        return radiusTwo * radiusTwo * M_PI;
    } 
    if (distance + radiusOne < radiusTwo) {
        return radiusOne * radiusOne * M_PI;
    }

	long long radiusOneSquare = radiusOne * radiusOne;
	long long radiusTwoSquare = radiusTwo * radiusTwo;
	long long distanceSquare = distance * distance;

	double phi = (acos((radiusOneSquare + distanceSquare - radiusTwoSquare) / (2 * radiusOne * distance))) * 2;
	double theta = (acos((radiusTwoSquare + distanceSquare - radiusOneSquare) / (2 * radiusTwo * distance))) * 2;
	double a1 = 0.5 * theta * radiusTwoSquare - 0.5 * radiusTwoSquare * sin(theta);
	double a2 = 0.5 * phi * radiusOneSquare - 0.5 * radiusOneSquare * sin(phi);

	return a1 + a2;
}

double overlap_area_float(float r1, float r2, float dst)
{
	if (r1 == 0 || r2 == 0 || dst == 0)
	{
		return 0;
	}

	float r1_square = r1 * r1;
	float r2_square = r2 * r2;
	float dst_square = dst * dst;

	float res;

	// if (dst > r1 + r2)
    //     res = 0;
    // else if (dst <= (r1 - r2) && r1 >= r2)
    //     res = M_PI * r2_square;
    // else if (dst <= (r2 - r1) && r2 >= r1)
    //     res = M_PI * r1_square;
    // else {
		float acos_phi = (r1_square + dst_square - r2_square) / (2.0 * r1 * dst);
		float acos_theta = (r2_square + dst_square - r1_square) / (2.0 * r2 * dst);

		// if (acos_phi > 1)
		// 	acos_phi = 1;
		// else if (acos_phi < -1)
		// 	acos_phi = -1;

		// if (acos_theta > 1)
		// 	acos_theta = 1;
		// else if (acos_theta < -1)
		// 	acos_theta = -1;

		double phi = (acos(acos_phi)) * 2.0;
		double theta = (acos(acos_theta)) * 2.0;
		double a1 = 0.5 * theta * r2_square - 0.5 * r2_square * sin(theta);
		double a2 = 0.5 * phi * r1_square - 0.5 * r1_square * sin(phi);

		res = a1 + a2;
	// }

	return res;
}

unsigned char get_array_length(const char* arrayString, const size_t arrayStringLength)
{
	char copyOfArrayString[arrayStringLength];
	strcpy(copyOfArrayString, arrayString);

	unsigned char numberOfCommas = 0;
	for (unsigned short int i = 0; copyOfArrayString[i]; ++i)
	{
		if (copyOfArrayString[i] == ',')
		{
			numberOfCommas += 1;
		}
	}

	if (numberOfCommas != 0)
	{
		numberOfCommas += 1;
	}

	return numberOfCommas;
}
