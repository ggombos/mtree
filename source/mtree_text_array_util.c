/*
 * contrib/mtree_gist/mtree_text_array_util.c
 */

#include "mtree_text_array_util.h"
#include "mtree_util.h"

bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second)
{
	if (first->arrayLength != second->arrayLength)
	{
		return false;
	}

	for (unsigned char i = 0; i < first->arrayLength; ++i)
	{
		if (first->data[i] != second->data[i])
		{
			return false;
		}
	}

	return true;
}

bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second, float* distance)
{
	return *distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second, float* distance)
{
	return first->coveringRadius >= *distance + second->coveringRadius;
}

bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second, float* distance)
{
	return mtree_text_array_contains_distance(second, first, distance);
}

mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source)
{
	mtree_text_array* destination = (mtree_text_array*) palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

float get_text_array_distance(int size, mtree_text_array* entries[size], float distances[size][size], int i, int j)
{
	if (distances[i][j] == -1.0)
	{
		distances[i][j] = mtree_text_array_distance_internal(entries[i], entries[j]);
	}
	return distances[i][j];
}

float simple_text_array_distance(mtree_text_array* first, mtree_text_array* second)
{
	float sum = 0.0;
	unsigned char arrayLength = first->arrayLength;

	if (second->arrayLength < arrayLength)
	{
		arrayLength = second->arrayLength;
	}

	for (unsigned char i = 0; i < arrayLength; ++i)
	{
		unsigned char stringLength = strlen(first->data[i]);

		if (strlen(second->data[i]) > stringLength)
		{
			stringLength = strlen(second->data[i]);
		}

		for (unsigned char j = 0; j < stringLength; ++j)
		{
			if (first->data[i][j] != second->data[i][j])
			{
				sum += 1.0;
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
float weighted_text_array_distance(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	unsigned char numberOfMatchingTags = 0;
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	float sum = 0.0;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		int firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		int secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				isMatchingTag = true;
				numberOfMatchingTags++;

				free(secondDataStart);
				continue;
			}

			free(secondDataStart);
		}

		if (isMatchingTag)
		{
			sum += MIN_FLOAT(firstTagRelevance, secondTagRelevance);
		}

		free(firstDataStart);
	}

	sum /= 1.0 * (lengthOfFirstArray + lengthOfSecondArray - numberOfMatchingTags);

	return 100.0 - sum;
}

// Pearson correlation coefficient (PCC)
// def PCC_new(u,v):
float PCC(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float avg_first = 0.0;
	float avg_second = 0.0;
	float a = 0.0;
	float b = 0.0;
	float c = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	bool anyMatchingTag = false;

	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);
		
		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));
		avg_first += (float)firstTagRelevance;
	}
	avg_first = avg_first / (float)lengthOfFirstArray;

	for (unsigned char i = 0; i < lengthOfSecondArray; ++i)
	{
		char* secondData = calloc(strlen(second->data[i]) + 1, sizeof(char));
		char* secondDataStart = secondData;
		strcpy(secondData, second->data[i]);
		
		char* secondTagName = strtok_r(secondData, separator, &saveSecond);
		float secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));
		avg_second += (float)secondTagRelevance;
	}
	avg_second = avg_second / (float)lengthOfSecondArray;
		
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				isMatchingTag = true;
				anyMatchingTag = true;

				free(secondDataStart);
				continue;
			}

		}
		free(firstDataStart);

		if (isMatchingTag)
		{
			a += (firstTagRelevance - avg_first) * (secondTagRelevance - avg_second);
			b += (firstTagRelevance - avg_first) * (firstTagRelevance - avg_first);
			c += (secondTagRelevance - avg_second) * (secondTagRelevance - avg_second);
			// elog(INFO,"firstRel %f firstAvg %f secRel %f secAvg %f",firstTagRelevance, avg_first, secondTagRelevance, avg_second);
			// elog(INFO,"a %f b %f c %f sb %f sc %f dist %f",a,b,c,sqrt(b),sqrt(c), (a / (sqrt(b)*sqrt(c))));
		}
	}
	
	if ((sqrtf(b)*sqrtf(c)) == 0.0) {
		elog(INFO,"dist :( a %f b %f c %f",a,b,c );
		return 1.0;
	} else {
		elog(INFO,"dist %f",fabs( a / (sqrtf(b)*sqrtf(c)) ));
		// return fabs( a / (sqrtf(b)*sqrtf(c)) );
		return ( a / (sqrtf(b)*sqrtf(c)) )+1.0;
	}
}

// Cosine 
// def cosine(u,v):
float Cosine(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float a = 0.0;
	float b = 0.0;
	float c = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;


	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);
		
		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = atoi(strtok_r(NULL, separator, &saveFirst));
		b += (float)firstTagRelevance*(float)firstTagRelevance;
	}
	b = sqrtf(b);
	
	for (unsigned char i = 0; i < lengthOfSecondArray; ++i)
	{
		char* secondData = calloc(strlen(second->data[i]) + 1, sizeof(char));
		char* secondDataStart = secondData;
		strcpy(secondData, second->data[i]);
		
		char* secondTagName = strtok_r(secondData, separator, &saveSecond);
		float secondTagRelevance = atoi(strtok_r(NULL, separator, &saveSecond));
		c += (float)secondTagRelevance*(float)secondTagRelevance;
	}
	c = sqrtf(c);
		
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		bool isMatchingTag = false;
		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				isMatchingTag = true;

				free(secondDataStart);
				continue;
			}

		}
		free(firstDataStart);

		if (isMatchingTag)
		{
			a += firstTagRelevance * secondTagRelevance;
			// elog(INFO,"firstRel %f firstAvg %f secRel %f secAvg %f",firstTagRelevance, avg_first, secondTagRelevance, avg_second);
			// elog(INFO,"a %f b %f c %f sb %f sc %f dist %f",a,b,c,sqrt(b),sqrt(c), (a / (sqrt(b)*sqrt(c))));
		}
	}
	// elog(INFO,"a %f b %f c %f dist %f",a,b,c, (a / (b*c)));
	
	if (b == 0.0 || c == 0.0) {
		return 1.0;
	} else {
		return 1.0 - (a / (b*c));
	}
}

// Jaccard
// def Jaccard(u,v):
float Jaccard(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float a = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	
	
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				a++;

				free(secondDataStart);
				continue;
			}

		}
		free(firstDataStart);
	}
	
	return 1.0 - (a / ((float)lengthOfFirstArray + (float)lengthOfSecondArray - a));
	
}

// weighted Jaccard
// def WJaccard(u,v):
float ExtendedJaccard(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float a = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	
	
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				a += (firstTagRelevance*secondTagRelevance) / ( powf(firstTagRelevance,2) + powf(secondTagRelevance,2) - (firstTagRelevance*secondTagRelevance) ) ;

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
float TMJ(mtree_text_array* first, mtree_text_array* second)
{
	
}
	
// Mean Squablue Difference (MSD)
// def msd(u,v):
float MSD(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float a = 0.0;
	float b = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	
	
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				b++;
				a += powf((firstTagRelevance-secondTagRelevance)/100.0,2);

				free(secondDataStart);
				continue;
			}

		}
		free(firstDataStart);
	}
	
	if (b == 0.0) {
		return 1.0;
	} else {
		return a/b;
	}
}
	
// RAtio-based
// def ra(u,v):
float RA(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float a = 0.0;
	float b = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	
	
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				b++;
				a += fmin(firstTagRelevance,secondTagRelevance) / fmax(firstTagRelevance,secondTagRelevance);

				free(secondDataStart);
				continue;
			}

		}
		free(firstDataStart);
	}
	
	if (b == 0.0) {
		return 1.0;
	} else {
		return 1.0 - a/b;
	}
}
	
// Relevant Jaccard mean square distance
float RJMS(mtree_text_array* first, mtree_text_array* second)
{
	
}

float Euclidean(mtree_text_array* first, mtree_text_array* second)
{
	unsigned char lengthOfFirstArray = first->arrayLength;
	unsigned char lengthOfSecondArray = second->arrayLength;
	float a = 0.0;
	
	char* separator = "###";
	char* saveFirst;
	char* saveSecond;
	
	
	for (unsigned char i = 0; i < lengthOfFirstArray; ++i)
	{
		char* firstData = calloc(strlen(first->data[i]) + 1, sizeof(char));
		char* firstDataStart = firstData;
		strcpy(firstData, first->data[i]);

		char* firstTagName = strtok_r(firstData, separator, &saveFirst);
		float firstTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveFirst));

		float secondTagRelevance;
		for (unsigned char j = 0; j < lengthOfSecondArray; ++j)
		{
			char* secondData = calloc(strlen(second->data[j]) + 1, sizeof(char));
			char* secondDataStart = secondData;
			strcpy(secondData, second->data[j]);

			char* secondTagName = strtok_r(secondData, separator, &saveSecond);
			secondTagRelevance = (float)atoi(strtok_r(NULL, separator, &saveSecond));

			if (strcmp(firstTagName, secondTagName) == 0)
			{
				a += powf(firstTagRelevance - secondTagRelevance,2);

				free(secondDataStart);
				continue;
			}

		}
		free(firstDataStart);
	}
	
	return 1.0 - (1.0 / ( 1.0 + sqrtf(a)));
	
}

float Hamming(mtree_text_array* first, mtree_text_array* second)
{
	
}

float Manhatan(mtree_text_array* first, mtree_text_array* second)
{
	
}

float SimED(mtree_text_array* first, mtree_text_array* second)
{
	
}


float mtree_text_array_distance_internal(mtree_text_array* first, mtree_text_array* second)
{
	// return simple_text_array_distance(first, second);
	// return weighted_text_array_distance(first, second);
	
	// return PCC(first, second);
	// return Cosine(first,second);
	// nem foglalkozik a relevanciaval
	// return Jaccard(first,second);
	// az a probléma hogy nem foglalkozik a nem kozos elemekkel
	// return ExtendedJaccard(first,second);
	// az a probléma hogy nem foglalkozik a nem kozos elemekkel
	// return MSD(first,second);
	// return RA(first,second);
	return Euclidean(first,second);
}
