# float4_array

## 1

Input size is maybe bad when allocating memory (int64 instead of float).

# int8_array

## 1

When querying a table for the first time it returns a kind-of strange result - one, or some of the int8 values are looking like it has overflowed, but this is not the case.

## 2

Boundaries are not working -> instead of 9223372036854775807 it tells 65535.

## 3

Input size is maybe bad when allocating memory (int64 * 2 instead of int64).

# text_array

Function weighted_text_distance kills PostgreSQL.
