#include <stdio.h>
#include "../utils/bake.h"

int main() {
    size_t size;
    
    int* result = onePone(&size);
    printf("result: 1+1 = %d\n", *result);
    
    result = onePtow(&size);
    printf("result: 1+2 = %d\n", *result);

    printf("result: 1+10 : %f\n", onePten());

    return 0;
}