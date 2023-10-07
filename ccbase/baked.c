#include <stdlib.h>
#include <string.h>

void* onePone(size_t* size) {
   size[0] = 4;
   char data[] = {2, 0, 0, 0, };
   void* out = malloc(4);
   memmove(out, data, 4);
   return out;
}

void* onePtow(size_t* size) {
   size[0] = 4;
   char data[] = {3, 0, 0, 0, };
   void* out = malloc(4);
   memmove(out, data, 4);
   return out;
}

float onePten(size_t* size) {
   char data[] = {0, 0, 48, 65, -112, -118, };
   return *((float*)data);
}

