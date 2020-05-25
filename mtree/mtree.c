/*
 * mus_type_static.c
 *
 *  Created on: March xx, 2019
 *      Author: Donko Istvan
 */

#include <stdio.h>
#include "postgres.h"
#include "c.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "access/gist.h"
#include "access/skey.h"
#include "utils/elog.h"
#include "utils/palloc.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"
#include <math.h>
#include <limits.h>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */
#include <utils/array.h>
#include <utils/lsyscache.h>
#include <time.h>
#include <stdbool.h>

#include "access/genam.h"
#include "access/gist_private.h"

#define RED(text)    "\033[0;31m" text "\033[0m"
#define GREEN(text)  "\033[0;32m" text "\033[0m"
#define YELLOW(text) "\033[0;33m" text "\033[0m"
#define BLUE(text)   "\033[0;34m" text "\033[0m"

#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
  char vl_len_[4];
  int parentDistance;
  int coveringRadius;
  char vl_dat[FLEXIBLE_ARRAY_MEMBER];
}
mus_gist;

#define OWNHDRSZ (3 * sizeof(int))
#define DatumGetMusGist(x) ((mus_gist *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MUS_GIST_P(x) DatumGetMusGist(PG_GETARG_DATUM(x))
#define PG_RETURN_MUS_GIST_P(x) PG_RETURN_POINTER(x)

typedef enum {
  UnionFirst,
  UnionBest
}
UnionStrategy;

typedef enum {
  PicksplitRandom,
  PicksplitFirstTwo,
  PicksplitMaxDistanceFromFirst,
  PicksplitMaxDistancePair,
  PicksplitSamplingMinCoveringSum,
  PicksplitSamplingMinCoveringMax,
  PicksplitSamplingMinOverlapArea,
  PicksplitSamplingMinAreaSum
}
PicksplitStrategy;

UnionStrategy unionStrategy = UnionBest;
PicksplitStrategy picksplitStrategy = %picksplitStrategy%;
// PicksplitStrategy picksplitStrategy = PicksplitSamplingMinCoveringSum;

#define SameStrategyNumber      1 //  =
#define OverlapStrategyNumber   2 // #&#
#define ContainsStrategyNumber  3 // #>#
#define ContainedStrategyNumber 4 // #<#

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(mus_gist_in);

Datum mus_gist_in(PG_FUNCTION_ARGS) {
  char *str = PG_GETARG_CSTRING(0); 
  // elog(INFO,":::%s",str);
  
  int cr = 0;

  if (isdigit(str[0])) {
    char *num = str;

    while(isdigit(str[0])) { str++; }

    str[0] = '\0';
    str++;

    cr = atoi(num);
  }

  mus_gist *result = (mus_gist *) palloc(OWNHDRSZ + strlen(str) * sizeof(char) + 1);
  result->coveringRadius = cr;
  result->parentDistance = 0;

  SET_VARSIZE(result, OWNHDRSZ + strlen(str) * sizeof(char) + 1);
  
  strcpy(result->vl_dat, str);
  result->vl_dat[strlen(str)] = '\0';
  // elog(NOTICE, "in_szof | (vh): %i | (oh): %i", VARHDRSZ, OWNHDRSZ);
  // elog(NOTICE, "in_end | (cr): %i", result->coveringRadius);
  PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(mus_gist_out);

Datum mus_gist_out(PG_FUNCTION_ARGS) {
  mus_gist *mus = PG_GETARG_MUS_GIST_P(0);

  // mus_gist *gist_mus = (mus_gist *) PG_GETARG_POINTER(0);
  // elog(NOTICE, "before | (cr): %i", mus->coveringRadius + mus->parentDistance);
  // mus_gist *mus = PG_DETOAST_DATUM(gist_mus->data);
  // elog(NOTICE, "after");

  //char *result;
  //result = psprintf("%s", str);
  //result = psprintf("%c", mus->vl_dat[0]);
 
  char *result;

  if (mus->coveringRadius == 0) {
    result = psprintf("%s", mus->vl_dat);
  } else {
    result = psprintf("%d|%s", mus->coveringRadius, mus->vl_dat);
  }
 
  PG_RETURN_CSTRING(result);
  //PG_RETURN_CSTRING(mus->vl_dat);
  //PG_RETURN_CSTRING("test");
}

/*****************************************************************************
 * Binary Input/Output functions of mus_gist
 *****************************************************************************/


mus_gist *mus_gist_deep_copy(mus_gist *source) {
  mus_gist *destination = (mus_gist *) palloc(VARSIZE_ANY(source)); 
  
  memcpy(destination, source, VARSIZE_ANY(source));

  return destination; 
}

/*****************************************************************************
 * New Operators of mus_gist
 *****************************************************************************/

bool internal_mus_gist_same(mus_gist *mus1, mus_gist *mus2) {
  return !strcmp(mus1->vl_dat, mus2->vl_dat);
}

PG_FUNCTION_INFO_V1(mus_gist_same);

Datum mus_gist_same(PG_FUNCTION_ARGS) {
  mus_gist *mus1 = PG_GETARG_MUS_GIST_P(0);
  mus_gist *mus2 = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_mus_gist_same(mus1, mus2);
  PG_RETURN_BOOL(result);
}


int string_distance(char* a, char* b) {
  int lengthA = strlen(a);
  int lengthB = strlen(b);

  int x, y, lastdiag, olddiag;
  int column[lengthA + 1];
 
  column[0] = 0;
  for (y = 1; y <= lengthA; y++) {
    column[y] = y;
  }
  
  for (x = 1; x <= lengthB; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= lengthA; y++) {
      olddiag = column[y];
      column[y] = MIN3(
        column[y] + 1,
        column[y - 1] + 1,
        lastdiag + (a[y - 1] == b[x - 1] ? 0 : 1)
      );
      lastdiag = olddiag;
    }
  }

  return column[lengthA];
}

int internal_mus_gist_distance(mus_gist *mus1, mus_gist *mus2) {
  char *data1 = mus1->vl_dat;
  char *data2 = mus2->vl_dat;

  return string_distance(data1, data2);
}

PG_FUNCTION_INFO_V1(mus_gist_distance);

// Datum mus_gist_distance(mus_gist *mus1, mus_gist *mus2) {
Datum mus_gist_distance(PG_FUNCTION_ARGS) {
  mus_gist *mus1 = PG_GETARG_MUS_GIST_P(0);
  mus_gist *mus2 = PG_GETARG_MUS_GIST_P(1);

  int result = internal_mus_gist_distance(mus1, mus2);
  PG_RETURN_INT32(result);
}

// ---

int internal_text_distance(text *text1, text *text2) {
  return string_distance(text1->vl_dat, text2->vl_dat);
}

PG_FUNCTION_INFO_V1(text_distance);

Datum text_distance(PG_FUNCTION_ARGS) {
  text *text1 = PG_GETARG_TEXT_P(0);
  text *text2 = PG_GETARG_TEXT_P(1);

  PG_RETURN_INT32(internal_text_distance(text1, text2));
}

// ---

bool internal_mus_gist_overlap_d(mus_gist *mus1, mus_gist *mus2, int distance) {
  return distance <= mus1->coveringRadius + mus2->coveringRadius;
}

bool internal_mus_gist_overlap(mus_gist *mus1, mus_gist *mus2) {
  int distance = internal_mus_gist_distance(mus1, mus2);
  return internal_mus_gist_overlap_d(mus1, mus2, distance);
}

PG_FUNCTION_INFO_V1(mus_gist_overlap);

Datum mus_gist_overlap(PG_FUNCTION_ARGS) {
  mus_gist *mus1 = PG_GETARG_MUS_GIST_P(0);
  mus_gist *mus2 = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_mus_gist_overlap(mus1, mus2);
  PG_RETURN_BOOL(result);
}

// ---

bool contains_d(mus_gist *big, mus_gist *small, int distance) {
  return big->coveringRadius >= distance + small->coveringRadius;
}

bool contains(mus_gist *big, mus_gist *small) {
  int distance = internal_mus_gist_distance(big, small);
  return contains_d(big, small, distance);
}

// ---

bool internal_mus_gist_contains_d(mus_gist *mus1, mus_gist *mus2, int distance) {
  return contains_d(mus1, mus2, distance);
}

bool internal_mus_gist_contains(mus_gist *mus1, mus_gist *mus2) {
  return contains(mus1, mus2);
}

PG_FUNCTION_INFO_V1(mus_gist_contains);

Datum mus_gist_contains(PG_FUNCTION_ARGS) {
  mus_gist *mus1 = PG_GETARG_MUS_GIST_P(0);
  mus_gist *mus2 = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_mus_gist_contains(mus1, mus2);
  PG_RETURN_BOOL(result);
}

// ---

bool internal_mus_gist_contained_d(mus_gist *mus1, mus_gist *mus2, int distance) {
  return contains_d(mus2, mus1, distance);
}

bool internal_mus_gist_contained(mus_gist *mus1, mus_gist *mus2) {
  return contains(mus2, mus1);
}

PG_FUNCTION_INFO_V1(mus_gist_contained);

Datum mus_gist_contained(PG_FUNCTION_ARGS) {
  mus_gist *mus1 = PG_GETARG_MUS_GIST_P(0);
  mus_gist *mus2 = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_mus_gist_contained(mus1, mus2);
  PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Operator class of mus_gist for defining the GIST index
 *****************************************************************************/

PG_FUNCTION_INFO_V1(mus_gist_compress);

Datum mus_gist_compress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

PG_FUNCTION_INFO_V1(mus_gist_decompress);

Datum mus_gist_decompress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

PG_FUNCTION_INFO_V1(mus_gist_consistent);

Datum mus_gist_consistent(PG_FUNCTION_ARGS) {
  // elog(NOTICE, "#consistent start");
  GISTENTRY *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
  mus_gist *query = PG_GETARG_MUS_GIST_P(1);
  StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
  //Oid subtype = PG_GETARG_OID(3);
  bool *recheck = (bool *) PG_GETARG_POINTER(4);
  mus_gist *key = DatumGetMusGist(entry->key);
  bool retval;

  int distance = internal_mus_gist_distance(key, query);
  
  if (GIST_LEAF(entry)) {
    *recheck = false;
   
    switch(strategy) {
      case SameStrategyNumber:
        {
          retval = internal_mus_gist_same(key, query);
          // elog(NOTICE, "leaf-same");
          break;
        }
      
      case OverlapStrategyNumber:
        {
          retval = internal_mus_gist_overlap_d(key, query, distance);
          break;
        }

      case ContainsStrategyNumber:
        {
          retval = internal_mus_gist_contains_d(key, query, distance);
          break;
        }

      case ContainedStrategyNumber:
        {
          retval = internal_mus_gist_contained_d(key, query, distance);
          // elog(NOTICE, "leaf-contained-check");
          break;
        }

      default:
        {
          elog(ERROR, "Invalid consistent strategy: %d", strategy);
          break;
        }
    }

  } else {
    switch(strategy) {
      case SameStrategyNumber:
        {
          retval = internal_mus_gist_contains_d(key, query, distance);
          // elog(NOTICE, "same-node: %d", retval);
          *recheck = true;
          break;
        }
      
      case OverlapStrategyNumber:
        {
          retval = internal_mus_gist_overlap_d(key, query, distance);
          *recheck = !internal_mus_gist_contained_d(key, query, distance);
          break;
        }

      case ContainsStrategyNumber:
        {
          retval = internal_mus_gist_contains_d(key, query, distance);
          *recheck = true;
          break;
        }

      case ContainedStrategyNumber:
        {
          retval = internal_mus_gist_overlap_d(key, query, distance);
          *recheck = !internal_mus_gist_contained_d(key, query, distance);
          // elog(NOTICE, "contained-node-check %d", retval);
          break;
        }

      default:
        {
          elog(ERROR, "Invalid consistent strategy: %d", strategy);
          break;
        }
    }
    
  }
  
  PG_RETURN_BOOL(retval);
}

void initDistances(int size, int distances[size][size]) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      distances[i][j] = -1;
    }
  }
}

int getDistance(int size, mus_gist *entries[size], int distances[size][size], int i, int j) {
  if (distances[i][j] == -1) {
    distances[i][j] = internal_mus_gist_distance(entries[i], entries[j]);
  }

  return distances[i][j];
}

PG_FUNCTION_INFO_V1(gist_union);

Datum gist_union(PG_FUNCTION_ARGS) {
  GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
  GISTENTRY *ent = entryvec->vector;
  int numranges = entryvec->n;

  mus_gist *entries[numranges];
  for (int i = 0; i < numranges; i++) {
    entries[i] = DatumGetMusGist(ent[i].key);
  } 

  int searchRange;
  
  switch(unionStrategy) {
    case UnionFirst:
      {
        searchRange = 1;

        break;
      }

    case UnionBest:
      {
        searchRange = numranges;

        break;
      }
    
    default:
      {
        elog(ERROR, "Invalid union strategy: %d", unionStrategy);

        break;
      }
  }

  int coveringRadii[searchRange];

  for (int i = 0; i < searchRange; i++) {
    coveringRadii[i] = 0;

    for (int j = 0; j < numranges; j++) {
      int distance = internal_mus_gist_distance(entries[i], entries[j]);
      int newCoveringRadius = distance + entries[j]->coveringRadius;

      if (coveringRadii[i] < newCoveringRadius) {
        coveringRadii[i] = newCoveringRadius;
      }
    }
  }
 
  int minimumIndex = 0;

  for (int i = 1; i < searchRange; i++) {
    if (coveringRadii[i] < coveringRadii[minimumIndex]) {
      minimumIndex = i;
    }
  }

  mus_gist *out = mus_gist_deep_copy(entries[minimumIndex]);
  out->coveringRadius = coveringRadii[minimumIndex];

  PG_RETURN_MUS_GIST_P(out);
}

PG_FUNCTION_INFO_V1(mus_gist_penalty);

Datum mus_gist_penalty(PG_FUNCTION_ARGS) {
  // elog(WARNING, "gist_penalty start");
  
  GISTENTRY *origentry = (GISTENTRY *) PG_GETARG_POINTER(0);
  GISTENTRY *newentry = (GISTENTRY *) PG_GETARG_POINTER(1);
  float *penalty = (float *) PG_GETARG_POINTER(2);
  mus_gist *orig = DatumGetMusGist(origentry->key);
  mus_gist *new = DatumGetMusGist(newentry->key);
  
  int distance = internal_mus_gist_distance(orig, new);
  int newCoveringRadius = distance + new->coveringRadius;

  *penalty = (newCoveringRadius < orig->coveringRadius ? 0 : newCoveringRadius - orig->coveringRadius);
  PG_RETURN_POINTER(penalty);
}

double overlapArea(int r1, int r2, int d) {
  if (r1 == 0 || r2 == 0 || d == 0) {
    return 0;
  }

  int r1sq = r1 * r1;
  int r2sq = r2 * r2;
  int dsq = d * d;

  double phi = (acos((r1sq + dsq - r2sq) / (2 * r1 * d))) * 2;
  double theta = (acos((r2sq + dsq - r1sq) / (2 * r2 * d))) * 2;
  double a1 = 0.5 * theta * r2sq - 0.5 * r2sq * sin(theta);
  double a2 = 0.5 * phi * r1sq - 0.5 * r1sq * sin(phi);

  return a1 + a2;
}

PG_FUNCTION_INFO_V1(gist_picksplit);

Datum gist_picksplit(PG_FUNCTION_ARGS) {
  // elog(NOTICE, "#picksplit start");
  // elog(WARNING, "gist_picksplit start");
  
  GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
  GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);
  OffsetNumber maxoff = entryvec->n - 1;
  GISTENTRY  *ent = entryvec->vector;
  OffsetNumber nbytes;
  OffsetNumber *left, *right;
  GISTENTRY **raw_entryvec;

  maxoff = entryvec->n - 1;
  nbytes = (maxoff + 1) * sizeof(OffsetNumber);

  v->spl_left = (OffsetNumber *) palloc(nbytes);
  left = v->spl_left;
  v->spl_nleft = 0;

  v->spl_right = (OffsetNumber *) palloc(nbytes);
  right = v->spl_right;
  v->spl_nright = 0;

  // Collecting entries

  mus_gist *entries[maxoff];
  for (OffsetNumber i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i)) { 
    entries[i - FirstOffsetNumber] = DatumGetMusGist(entryvec->vector[i].key);
  } 

  int distances[maxoff][maxoff];
  initDistances(maxoff, distances);

  // Selecting objects to promote

  int leftIndex, rightIndex;

  switch (picksplitStrategy) {
    case PicksplitRandom:
      {

        leftIndex = random() % (maxoff - 1);
        rightIndex = (leftIndex + 1) + (random() % (maxoff - leftIndex - 1));

        break;
      }

      //

    case PicksplitFirstTwo:
      {
        leftIndex = 0;
        rightIndex = 1;

        break;
      }

      //

    case PicksplitMaxDistanceFromFirst:
      {
        int rightCandidateIndex;
        int maxDistance = -1;

        for (int r = 0; r < maxoff; r++) { 
          int d = getDistance(maxoff, entries, distances, 0, r); 

          if (d > maxDistance) {
            maxDistance = d;
            rightCandidateIndex = r;
          }
        }

        leftIndex = 0;
        rightIndex = rightCandidateIndex;

        break;
      }

      //

    case PicksplitMaxDistancePair:
      {
        int leftCandidateIndex, rightCandidateIndex;
        int maxDistance = -1;

        for (OffsetNumber l = 0; l < maxoff; l++) { 
          for (OffsetNumber r = l; r < maxoff; r++) { 
            int d = getDistance(maxoff, entries, distances, l, r); 

            if (d > maxDistance) {
              maxDistance = d;
              leftCandidateIndex = l;
              rightCandidateIndex = r;
            }
          }
        }

        leftIndex = 0;
        rightIndex = rightCandidateIndex;

        break;
      }

      //

    case PicksplitSamplingMinCoveringSum:
      {
        int leftCandidateIndex, rightCandidateIndex;
        
        // elog(NOTICE, "maxoff: %d", maxoff);
        
        int trialCount = 100;
        int minCoveringSum = -1;

        for (int i = 0; i < trialCount; i++) {
          leftCandidateIndex = random() % (maxoff - 1);
          rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxoff - leftCandidateIndex - 1));
          
          int leftRadius = 0, rightRadius = 0;

          for (int currentIndex = 0; currentIndex < maxoff; currentIndex++) { 
            int leftDistance = getDistance(maxoff, entries, distances, leftCandidateIndex, currentIndex); 
            int rightDistance = getDistance(maxoff, entries, distances, rightCandidateIndex, currentIndex); 

            if (leftDistance < rightDistance) {
              if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
                leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
              }
            } else {
              if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
                rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
              }
            }
          }

          if (minCoveringSum == -1 || leftRadius + rightRadius < minCoveringSum) {
            minCoveringSum = leftRadius + rightRadius;

            leftIndex = leftCandidateIndex;
            rightIndex = rightCandidateIndex;
          }
        }

        break;
      }
      
      //

    case PicksplitSamplingMinCoveringMax:
      {
        int leftCandidateIndex, rightCandidateIndex;
        
        // elog(NOTICE, "maxoff: %d", maxoff);

        int trialCount = 100;
        int minCoveringMax = -1;
 
        for (int i = 0; i < trialCount; i++) {
          leftCandidateIndex = random() % (maxoff - 1);
          rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxoff - leftCandidateIndex - 1));
          
          int leftRadius = 0, rightRadius = 0;

          for (int currentIndex = 0; currentIndex < maxoff; currentIndex++) { 
            int leftDistance = getDistance(maxoff, entries, distances, leftCandidateIndex, currentIndex); 
            int rightDistance = getDistance(maxoff, entries, distances, rightCandidateIndex, currentIndex); 

            if (leftDistance < rightDistance) {
              if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
                leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
              }
            } else {
              if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
                rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
              }
            }
          }

          if (minCoveringMax == -1 || MAX2(leftRadius, rightRadius) < minCoveringMax) {
            minCoveringMax = MAX2(leftRadius, rightRadius);

            leftIndex = leftCandidateIndex;
            rightIndex = rightCandidateIndex;
          }
        }

        break;
      }

      // 

    case PicksplitSamplingMinOverlapArea:
      {
        int leftCandidateIndex, rightCandidateIndex;
        
        // elog(NOTICE, "maxoff: %d", maxoff);

        int trialCount = 100;
        int minOverlapArea = -1;
 
        for (int i = 0; i < trialCount; i++) {
          leftCandidateIndex = random() % (maxoff - 1);
          rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxoff - leftCandidateIndex - 1));
         
          int distance = getDistance(maxoff, entries, distances, leftCandidateIndex, rightCandidateIndex);
          int leftRadius = 0, rightRadius = 0;

          for (int currentIndex = 0; currentIndex < maxoff; currentIndex++) { 
            int leftDistance = getDistance(maxoff, entries, distances, leftCandidateIndex, currentIndex); 
            int rightDistance = getDistance(maxoff, entries, distances, rightCandidateIndex, currentIndex); 

            if (leftDistance < rightDistance) {
              if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
                leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
              }
            } else {
              if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
                rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
              }
            }
          }

          double currentOverlapArea = overlapArea(leftRadius, rightRadius, distance);
          if (minOverlapArea == -1 || currentOverlapArea < minOverlapArea) {
            minOverlapArea = currentOverlapArea;

            leftIndex = leftCandidateIndex;
            rightIndex = rightCandidateIndex;
          }
        }

        break;
      }

      // 

    case PicksplitSamplingMinAreaSum:
      {
        int leftCandidateIndex, rightCandidateIndex;
        
        // elog(NOTICE, "maxoff: %d", maxoff);

        int trialCount = 100;
        int minSumArea = -1;
 
        for (int i = 0; i < trialCount; i++) {
          leftCandidateIndex = random() % (maxoff - 1);
          rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxoff - leftCandidateIndex - 1));
          
          int leftRadius = 0, rightRadius = 0;

          for (int currentIndex = 0; currentIndex < maxoff; currentIndex++) { 
            int leftDistance = getDistance(maxoff, entries, distances, leftCandidateIndex, currentIndex); 
            int rightDistance = getDistance(maxoff, entries, distances, rightCandidateIndex, currentIndex); 

            if (leftDistance < rightDistance) {
              if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
                leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
              }
            } else {
              if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
                rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
              }
            }
          }

          int currentSumArea = leftRadius * leftRadius + rightRadius * rightRadius;
          if (minSumArea == -1 || currentSumArea < minSumArea) {
            minSumArea = currentSumArea;

            leftIndex = leftCandidateIndex;
            rightIndex = rightCandidateIndex;
          }
        }

        break;
      }

      // 

    default:
      {

        elog(ERROR, "Invalid picksplit strategy: %d", picksplitStrategy);

        break;
      }
  }

  mus_gist *unionL = mus_gist_deep_copy(entries[leftIndex]);
  mus_gist *unionR = mus_gist_deep_copy(entries[rightIndex]);
  
  mus_gist *current;
  for (OffsetNumber i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i)) {
    current = entries[i - 1];

    /*
     * Choose where to put the index entries and update unionL and unionR
     * accordingly. Append the entries to either v_spl_left or
     * v_spl_right, and care about the counters.
     */

    int distanceL = getDistance(maxoff, entries, distances, leftIndex, i - 1); 
    int distanceR = getDistance(maxoff, entries, distances, rightIndex, i - 1); 
    
    if (distanceL < distanceR)
    {
      if (distanceL + current->coveringRadius > unionL->coveringRadius) {
        unionL->coveringRadius = distanceL + current->coveringRadius;
      }

      *left = i;
      ++left;
      ++(v->spl_nleft);
    }
    else
    {
      if (distanceR + current->coveringRadius > unionR->coveringRadius) {
        unionR->coveringRadius = distanceR + current->coveringRadius;
      }
      
      *right = i;
      ++right;
      ++(v->spl_nright);
    }
  }

  v->spl_ldatum = PointerGetDatum(unionL);
  v->spl_rdatum = PointerGetDatum(unionR);

  // ---

  PG_RETURN_POINTER(v);
}

PG_FUNCTION_INFO_V1(mus_gist_same_internal);

Datum mus_gist_same_internal(PG_FUNCTION_ARGS)
{
  mus_gist *mus1 = (mus_gist *) PG_GETARG_POINTER(0);
  mus_gist *mus2 = (mus_gist *) PG_GETARG_POINTER(1);
  bool *result = (bool *) PG_GETARG_POINTER(2);
  
  *result = (mus1->vl_dat[0] == mus2->vl_dat[0]);
  PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(mus_gist_distance_internal);

Datum mus_gist_distance_internal(PG_FUNCTION_ARGS)
{
  GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
  mus_gist  *query = PG_GETARG_MUS_GIST_P(1);
  StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
  /* Oid subtype = PG_GETARG_OID(3); */
  /* bool *recheck = (bool *) PG_GETARG_POINTER(4); */
  mus_gist  *key = DatumGetMusGist(entry->key);
  double retval;

  /*
   * determine return value as a function of strategy, key and query.
   */

  retval = internal_mus_gist_distance(query, key);

  PG_RETURN_FLOAT8(retval);
}
