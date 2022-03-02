# Supported data types

## Array types

### Syntax

```
VALID:
 a
[a]
 a,b
[a,b]

INVALID:

[]
[a,]
[a
```

TODO: Delete this part
#### Parse

```c
unsigned int arrayLength = 0;
unsigned int squareBracketCounter = 0;
for (unsigned int i = 0; i < inputString.length; ++i) {
    if (squareBracketCounter < 0) {
        ERROR
    } else if (inputString[i] == '[') {
        ++squareBracketCounter;
    } else if (inputString[i] == ']') {
        --squareBracketCounter;
    } else if (inputString[i] == ',') {
        ++arrayLength;
    }
}
if (squareBracketCounter != 0) {
    ERROR
}
```
