/*
 * contrib/mtree_gist/mtree_text_array_util.c
 */

#include "mtree_text_array_util.h"
#include "mtree_util.h"

float mtree_text_array_distance_internal(mtree_text_array* first, mtree_text_array* second) {
	return simple_text_distance(first, second);
	//return weighted_text_distance(first, second);
}

float weighted_text_distance(mtree_text_array* first, mtree_text_array* second) {
	float sum = 0.0;
	int sameTagCount = 0;
	char* save_ptr1;
	int lengthF = first->arrayLength;
	int lengthS = second->arrayLength;

	for (int xF = 0; xF < lengthF; ++xF) {

		// copy because the strtok kill the original string
		char* rest = calloc(strlen(first->data[xF]) + 1, sizeof(char));
		char* restPtr = rest;	// csak hogy fel tudjuk szabaditani
		strcpy(rest, first->data[xF]);

		char* tagF = strtok_r(rest, "###", &rest);
		int tagNumF = atoi(strtok_r(rest, "###", &rest));
		strtok_r(rest, "###", &rest);

		bool found = false;
		int tagNumS;
		for (int xS = 0; xS < lengthS; ++xS) {
			char* rest2 = calloc(strlen(second->data[xS]) + 1, sizeof(char));
			char* rest2Ptr = rest2;	// csak hogy fel tudjuk szabaditani
			strcpy(rest2, second->data[xS]);


			char* tagS = strtok_r(rest2, "###", &rest2);
			tagNumS = atoi(strtok_r(rest2, "###", &rest2));
			strtok_r(rest2, "###", &rest2);

			if (strcmp(tagF, tagS) == 0) {
				found = true;
				sameTagCount++;
				free(rest2Ptr);
				continue;
			}
			free(rest2Ptr);
		}
		free(restPtr);
		if (found) {
			sum += 100.0 - abs(tagNumF - tagNumS);
		}
	}
	// a nem talalhato tageket 100-zal szamolodnak
	// sum += 100.0*(lengthF+lengthS-(2*sameTagCount));
	sum /= 1.0 * (lengthF + lengthS - sameTagCount);

	return 100.0 - sum;
}


int simple_text_distance(mtree_text_array* first, mtree_text_array* second) {
	int sum = 0;
	int length = (int)sizeof(first);
	if ((int)sizeof(second) < length) {
		length = (int)sizeof(second);
	}
	for (int x = 0; x < length; ++x) {
		if (string_distance(first->data[x], second->data[x]) == 0) {
			sum++;
		}
	}

	return length - sum;
}

bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second) {
	if (first->arrayLength != second->arrayLength) {
		return false;
	}

	for (unsigned char i = 0; i < first->arrayLength; ++i) {
		if (first->data[i] != second->data[i]) {
			return false;
		}
	}

	return true;
}

bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second, float* distance) {
	return *distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second, float* distance) {
	return first->coveringRadius >= *distance + second->coveringRadius;
}

bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second, float* distance) {
	return mtree_text_array_contains_distance(second, first, distance);
}

mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source) {
	mtree_text_array* destination = (mtree_text_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

int get_text_array_distance(int size, mtree_text_array* entries[size], int distances[size][size], int i, int j) {
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_text_array_distance_internal(entries[i], entries[j]);
	}
	return distances[i][j];
}
