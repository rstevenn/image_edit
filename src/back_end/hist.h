#ifndef __HISTORY_H__
#define __HISTORY_H__


#include <stdio.h>
#include <stdlib.h>

#include "../shared_def.h"
#undef ERROR
#include <windows.h>
#undef ERROR

#include "../../config.h"
#include "../../ccbase/logs/log.h"
#include "../../ccbase/betterSDL/bsdl.h"


// structs
typedef struct {
    int id;
    unsigned char color_before[4];
    unsigned char color_after[4];
} pxl_change;


typedef struct {
    size_t size;
    size_t allocate;
    pxl_change* hist;        
} pxl_change_cache;


typedef struct pxl_modification_hist_struct{
    pxl_change cache;
    struct pxl_modification_hist_struct* next;
} pxl_modification_hist;


typedef struct {
    pxl_modification_hist* head;
    pxl_modification_hist* tail;
} pxl_modification_hist_span;


typedef struct modification_hist_struct{
    pxl_modification_hist_span span;
    struct modification_hist_struct* end;
} modification_hist;


// functions
pxl_change init_pxl_change(int id, unsigned char color_before[4], unsigned char after_color[4]);

pxl_change_cache* init_pxl_change_cache();
pxl_change_cache* add_pxl_change_cache(pxl_change_cache* data, pxl_change);
void free_pxl_change_cache(pxl_change_cache* data);




#endif // __HISTORY_H__
