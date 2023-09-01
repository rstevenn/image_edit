/*
    USAGE: 

*/
#ifndef __BAKE_H__
#define __BAKE_H__

#include <stdlib.h>


void* onePone(size_t* size);
void* onePtow(size_t* size);
float onePten(void);



#define Functions() BAKE(onePone) \
                    BAKE(onePtow) \
                    BAKETYPE(onePten, float)


#endif