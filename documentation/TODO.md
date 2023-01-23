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

# mtree_text_array

The distance function parametrization is only working for one type at a time.
The distance function is always overwritten by the latest index operator class.
One idea is to store the function name to the index - in a concatenated string,
if we know the name of the index in the function context (worst case, another
parameter).
