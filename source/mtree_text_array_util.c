/*
 * contrib/mtree_gist/mtree_text_array_util.c
 */

#include "mtree_text_array_util.h"

#include "mtree_util.h"

double mtree_text_array_outer_distance(mtree_text_array* first, mtree_text_array* second)
{
	return simple_text_array_distance(first, second);
	// return weighted_text_array_distance(first, second);

	// ez nem jo semmire
	//  double dist = PCC(first, second);
	//  az a probléma hogy nem foglalkozik a nem kozos elemekkel
	//  double dist = ExtendedJaccard(first,second);

	// double dist = Euclidean(first,second);
	// double dist = Hamming(first,second);
	// double dist = Manhatan(first,second);
	// double dist = SimED(first,second);
	// double dist = Cosine(first,second);
	// double dist = Jaccard(first,second);
	// double dist = TMJ(first,second);
	// double dist = MSD(first,second);
	double dist = RA(first, second);

	// ha a nem közös tagokkal is akarunk foglalkozni
	dist += (1.0 - dist) * notCoTagsDistance(first, second);
	if (dist > 1.0) {
		dist = 1.0;
	}

	return dist;
}

double mtree_text_array_full_distance(mtree_text_array* first, mtree_text_array* second)
{
	return simple_text_array_distance(first, second);
}

bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second)
{
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

bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second)
{
	double full_distance = mtree_text_array_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second)
{
	double full_distance = mtree_text_array_full_distance(first, second);
	return full_distance + second->coveringRadius < first->coveringRadius;
}

bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second)
{
	return mtree_text_array_contains_distance(second, first);
}

mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source)
{
	mtree_text_array* destination = (mtree_text_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

double get_text_array_distance(int size, mtree_text_array* entries[size], double distances[size][size], int i, int j)
{
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_text_array_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}

double simple_text_array_distance(mtree_text_array* first, mtree_text_array* second)
{
	double sum = 0.0;
	unsigned char arrayLength = first->arrayLength;

	if (second->arrayLength < arrayLength) {
		arrayLength = second->arrayLength;
	}

	for (unsigned char i = 0; i < arrayLength; ++i) {
		unsigned char stringLength = strlen(first->data[i]);

		if (strlen(second->data[i]) > stringLength) {
			stringLength = strlen(second->data[i]);
		}

		for (unsigned char j = 0; j < stringLength; ++j) {
			if (first->data[i][j] != second->data[i][j]) {
				sum += 1;
			}
		}
	}

	return arrayLength - sum;
}

#define MIN_FLOAT(x, y) (((x) < (y)) ? (1.0 * x) : (1.0 * y))

/*
 * This distance function is used for song similarity queries with the
 * Million Songs Dataset.
 */
double weighted_text_array_distance(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	unsigned char numberOfMatchingTags = 0;
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	double sum = 0.0;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		int firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		int secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				isMatchingTag = true;
				numberOfMatchingTags++;

				free(secondDataStart);
				continue;
			}

			free(secondDataStart);
		}

		if (isMatchingTag) {
			sum += MIN_FLOAT(firstTagRelevance, secondTagRelevance);
		}

		free(firstDataStart);
	}

	sum /= 1.0 * (lengthOfFirstArray + lengthOfSecondArray - numberOfMatchingTags);

	return 100.0 - sum;
}

// Pearson correlation coefficient (PCC)
// def PCC_new(u,v):
double PCC(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double avg_first = 0.0;
	double avg_second = 0.0;
	double a = 0.0;
	double b = 0.0;
	double c = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	// bool anyMatchingTag = false;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		// char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		// char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));
		avg_first += (double)firstTagRelevance;
	}
	avg_first = avg_first / (double)lengthOfFirstArray;

	for (unsigned char i = 0; i < lengthOfSecondArray; ++i) {
		char* secondData = calloc(strlen(second->data[i]) + 1, sizeof(char));
		// char* secondDataStart = secondData;
		strcpy(secondData, second->data[i]);

		// char* secondTagName = strtok_r(secondData, separator, &saveSecond);
		double secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));
		avg_second += (double)secondTagRelevance;
	}
	avg_second = avg_second / (double)lengthOfSecondArray;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				isMatchingTag = true;
				// anyMatchingTag = true;

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);

		if (isMatchingTag) {
			a += (firstTagRelevance - avg_first) * (secondTagRelevance - avg_second);
			b += (firstTagRelevance - avg_first) * (firstTagRelevance - avg_first);
			c += (secondTagRelevance - avg_second) * (secondTagRelevance - avg_second);
			// elog(INFO,"firstRel %f firstAvg %f secRel %f secAvg %f",firstTagRelevance, avg_first, secondTagRelevance,
			// avg_second); elog(INFO,"a %f b %f c %f sb %f sc %f dist %f",a,b,c,sqrt(b),sqrt(c), (a /
			// (sqrt(b)*sqrt(c))));
		}
	}
	// nem jo!!!!
	if ((sqrtf(b) * sqrtf(c)) == 0.0) {
		// elog(INFO,"dist :( a %f b %f c %f",a,b,c );
		return 1.0;
	} else {
		// elog(INFO,"dist %f",fabs( a / (sqrtf(b)*sqrtf(c)) ));
		// return fabs( a / (sqrtf(b)*sqrtf(c)) );
		//+1 mert -1 és 1 között van erdetileg
		return ((a / (sqrtf(b) * sqrtf(c))) + 1.0) / 2.0;
	}
}

// Cosine
// def cosine(u,v):
double Cosine(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;
	double c = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		// char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		// char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));
		b += (double)firstTagRelevance * (double)firstTagRelevance;
	}
	b = sqrtf(b);

	for (unsigned char i = 0; i < lengthOfSecondArray; ++i) {
		char* secondData = calloc(strlen(second->data[i]) + 1, sizeof(char));
		// char* secondDataStart = secondData;
		strcpy(secondData, second->data[i]);

		// char* secondTagName = strtok_r(secondData, separator, &saveSecond);
		double secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));
		c += (double)secondTagRelevance * (double)secondTagRelevance;
	}
	c = sqrtf(c);

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				isMatchingTag = true;

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);

		if (isMatchingTag) {
			a += firstTagRelevance * secondTagRelevance;
			// elog(INFO,"firstRel %f firstAvg %f secRel %f secAvg %f",firstTagRelevance, avg_first, secondTagRelevance,
			// avg_second); elog(INFO,"a %f b %f c %f sb %f sc %f dist %f",a,b,c,sqrt(b),sqrt(c), (a /
			// (sqrt(b)*sqrt(c))));
		}
	}
	// elog(INFO,"a %f b %f c %f dist %f",a,b,c, (a / (b*c)));

	if (b == 0.0 || c == 0.0) {
		return 1.0;
	} else {
		return 1.0 - (a / (b * c));
	}
}

// Jaccard
// def Jaccard(u,v):
double Jaccard(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		// double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		// double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			// secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				a++;

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	return 1.0 - (a / ((double)lengthOfFirstArray + (double)lengthOfSecondArray - a));
}

// weighted Jaccard
// def WJaccard(u,v):
double ExtendedJaccard(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				a += (firstTagRelevance * secondTagRelevance) /
					 (powf(firstTagRelevance, 2) + powf(secondTagRelevance, 2) -
					  (firstTagRelevance * secondTagRelevance));

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	return 1.0 - a;
}

// Triangle Multiplying Jaccard (TMJ)
// def tmj(u,v):
double TMJ(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;
	double c = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				a += powf(firstTagRelevance - secondTagRelevance, 2);
				b += powf(firstTagRelevance, 2);
				c += powf(secondTagRelevance, 2);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}
	if (isnan(1.0 - (Jaccard(first, second) * (1.0 - (sqrtf(a) / (sqrtf(b) + sqrtf(c))))))) {
		return 1.0;
	}

	return 1.0 - (Jaccard(first, second) * (1.0 - (sqrtf(a) / (sqrtf(b) + sqrtf(c)))));
}

// Mean Squablue Difference (MSD)
// def msd(u,v):
double MSD(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				b++;
				a += powf((firstTagRelevance - secondTagRelevance), 2);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	if (b == 0.0) {
		return 1.0;
	} else {
		return (a / b);
	}
}

// RAtio-based
// def ra(u,v):
double RA(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				b++;
				a += fmin(firstTagRelevance, secondTagRelevance) / fmax(firstTagRelevance, secondTagRelevance);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	if (b == 0.0) {
		return 1.0;
	} else {
		return 1.0 - a / b;
	}
}

double Euclidean(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				a += powf(firstTagRelevance - secondTagRelevance, 2);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	return 1.0 - (1.0 / (1.0 + sqrtf(a)));
}

double Hamming(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				b++;
				a += fabs((firstTagRelevance / (firstTagRelevance + secondTagRelevance)) - 0.5);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	if (b == 0.0) {
		return 1.0;
	} else {
		return (a / b);
	}
}

double Manhatan(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				b++;
				a += fabs(firstTagRelevance - secondTagRelevance);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	if (b == 0.0) {
		return 1.0;
	} else {
		// return (1.0-(1.0/100.0)*(a/b));   //similarity
		return (1.0 / 100.0) * (a / b);	 // distance
	}
}

double SimED(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double a = 0.0;
	double b = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				b++;
				a += powf(firstTagRelevance - secondTagRelevance, 2);

				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	// itt van egy kis elteres az eredeti keplettol
	if (b == 0.0) {
		return 1.0;
	} else {
		return (1.0 + sqrtf(a)) / (100.0 * sqrtf(b));
	}
}

double notCoTagsDistance(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	double b = 0.0;

	char* separator = "###";
	char* saveFirst;
	char* saveSecond;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i) {
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		// double firstTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveFirst));

		// double secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j) {
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			// secondTagRelevance = (double)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0) {
				b++;
				free(secondDataStart);
				continue;
			}
		}
		free(firstDataStart);
	}

	return (lengthOfFirstArray + lengthOfSecondArray - b) / (lengthOfFirstArray + lengthOfSecondArray);
}
