#include "hist.h"
#include "../../ccbase/logs/log.h"

// pxl_change
pxl_change init_pxl_change(int id, unsigned char color_before[4], unsigned char after_color[4]) {
    
    return (pxl_change) {id, color_before[0], color_before[1], color_before[2], color_before[3], 
                             after_color[0], after_color[1], after_color[2], after_color[3]};
}

// pxl_change_cache
pxl_change_cache* init_pxl_change_cache() {
    pxl_change_cache* out;

    NOTNULL(out = (pxl_change_cache*)malloc(sizeof(pxl_change_cache)), "Can't allocate memory")
    out->size     = 0;
    out->allocate = 0;

    return out;
}

pxl_change_cache* add_pxl_change_cache(pxl_change_cache* data, pxl_change in) {
    
    if (data->allocate == 0) {
        NOTNULL(data->hist = (pxl_change*)malloc(sizeof(pxl_change)), "Can't allocate memory")
        data->allocate = 1;
    } else if (data->allocate == data->size){
        NOTNULL(data->hist = (pxl_change*)realloc(data->hist, sizeof(pxl_change) * 2 * data->allocate), "Can't allocate memory")
        data->allocate *=2;
    }

    data->hist[data->size] = in;
    data->size++;

    return data;
}

void free_pxl_change_cache(pxl_change_cache* data) {
    free(data->hist);
    free(data);
}