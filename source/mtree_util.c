/*
 * contrib/mtree_gist/mtree_util.c
 */

#include "mtree_util.h"

#include "postgres.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Generates a simple deterministic hash for a string using the djb2 algorithm.
 *
 * This hash is used only for tie-breaking purposes, not for security.
 * It ensures that two different strings will produce different hashes.
 *
 * @param s Input string to hash.
 * @return 64-bit unsigned integer hash value.
 */
static unsigned long hash_string(const char *s)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *s++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

/**
 * @brief Computes a small numeric "tie breaker" value based on the hash of two strings.
 *
 * Why this is necessary:
 * 
 * String distance functions (like Levenshtein) can return the same value for
 * many different string pairs (e.g., "abc"→"abd" and "abc"→"acc" both have distance 1).
 * To ensure every pair gets a slightly different numeric result, we mix in a very small
 * pseudo-random fraction derived from the XOR of their hash values.
 *
 * Details:
 * 
 * - The XOR (^) combines both hashes into a single deterministic number.
 * - "& 0xFFFF" keeps only the lower 16 bits for simplicity.
 * - Dividing by 65536.0 converts it into a range [0.0, 1.0).
 * - Multiplying by 0.01 scales it down so it never dominates the main distance score.
 *
 * This makes the result unique and stable without affecting the main distance meaningfully.
 *
 * @param a First string.
 * @param b Second string.
 * @return Small fractional tie-breaker value in the range [0.0, 0.01).
 */
double calc_tie_breaker(const char* a, const char* b)
{
	unsigned long hash_a = hash_string(a);
    unsigned long hash_b = hash_string(b);

	return ((hash_a ^ hash_b) & 0xFFFF) / 65536.0 * 0.01;
}

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
