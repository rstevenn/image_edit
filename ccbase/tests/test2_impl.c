#include <stdio.h>
#include "../utils/bake.h"
#include <math.h>

void* onePone(size_t* size)
{
    int* out = malloc(sizeof(int));

    out[0] = 1+1; 
    size[0] = sizeof(int);
    return out;
}

void* onePtow(size_t* size)
{
    int* out = malloc(sizeof(int));

    out[0] = 1+2; 
    size[0] = sizeof(int);
    return out;
}

float onePten(void) {
    return 10+ cos(125)*cos(125) + sin(125)*sin(125);
}