#include "server.h"
#include <math.h>

float length(float dx, float dy) {
    return sqrtf((dx)*(dx) + (dy)*(dy));
}

void cubic_smoothing(mouse_pos_history_t state, int* x, int* y, size_t* size) {


    float alpha = 0.99;

    if (state.prev_size == 1) {
        INFO("SMOOTH 1")
        free(x); free(y);
        
        float px = (float)state.x[state.prev_size-1];
        float py = (float)state.y[state.prev_size-1];

        // push init pos
        NOTNULL(x = (int*)malloc(sizeof(int)), "can't allocate memory")
        NOTNULL(y = (int*)malloc(sizeof(int)), "can't allocate memory")
        x[0] = px; y[0] = py;

        size[0] = 1;
        INFO("SMOOTH 1 END")

    } else if (state.prev_size == 2) {

        free(x); free(y);
        
        float px = (float)state.x[state.prev_size-1];
        float py = (float)state.y[state.prev_size-1];

        // push init pos
        NOTNULL(x = (int*)malloc(sizeof(int)), "can't allocate memory")
        NOTNULL(y = (int*)malloc(sizeof(int)), "can't allocate memory")
        x[0] = px; y[0] = py;

        size[0] = 1;

    } else {
        INFO("SMOOTH >2")
        float dxa = (float)state.x[state.prev_size-2] - (float)state.x[state.prev_size-3]; 
        float dya = (float)state.y[state.prev_size-2] - (float)state.y[state.prev_size-3];

        float la = length(dxa, dya);

        float vx = dxa / la;
        float vy = dya / la;

        size_t i=1;
        free(x); free(y);
        
        float px = (float)state.x[state.prev_size-2];
        float py = (float)state.y[state.prev_size-2];

        // push init pos
        NOTNULL(x = (int*)malloc(sizeof(int)), "can't allocate memory")
        NOTNULL(y = (int*)malloc(sizeof(int)), "can't allocate memory")
        x[0] = px; y[0] = py;


        while (px != state.x[state.prev_size-1] ||
               py != state.y[state.prev_size-1]) {

            px += vx;
            py += vy; 

            float target_vx = state.x[state.prev_size-1] - px;
            float target_vy = state.y[state.prev_size-1] - py;

            vx = vx * alpha + (1-alpha) * state.x[state.prev_size-1];
            vy = vy * alpha + (1-alpha) * state.y[state.prev_size-1];
            alpha = alpha*alpha;

            i++;
            NOTNULL(x = (int*)realloc(x, sizeof(int)*i), "can't allocate memory")
            NOTNULL(y = (int*)realloc(y, sizeof(int)*i), "can't allocate memory")

            x[i-1] = px;
            y[i-1] = py;
            size[0] = i;
        }
    }
}


size_t pull_window_event(app_state_t* app_data, call_back_t* call_back) {

    if (app_data->event_nb == 0) { INFO("No event") return 0;} 

    // wait mutex
    CHECK(WaitForSingleObject(app_data->event_acces_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
    INFO("got mutex")

    call_back_t* queue = (call_back_t *)(app_data->call_backs);

    call_back[0] = queue[0];     
    
    app_data->event_nb--;
    if (app_data->event_nb > 0) {
        NOTNULL( (memmove(queue, queue + 1, sizeof(call_back_t)*(app_data->event_nb))), "Can't move callbacks to the front of the event queue")
        NOTNULL( (queue = (call_back_t *)realloc(queue, sizeof(call_back_t)*(app_data->event_nb))), "Can't realloc the event queue")
    } else {
        free(queue);
        queue = NULL;
    }

    app_data->call_backs = (void*)queue;
    size_t out = app_data->event_nb;

    // release
    CHECK(ReleaseMutex(app_data->event_acces_mtx) != 0, "Failed to release mutex");
    INFO("Mutex released")
    return out+1;

}


void callback_handler(call_back_t cb) {

    INFO("call_back start")
    switch (cb.type)
    {
        case CB_DRAW_PIXEL:
            
            (*cb.function.draw_pixel)(&cb.data.draw_pixel_data);
            break;
    
        default:
            break;
    }
    INFO("call_back end")
    
}


void draw_pxl(draw_pixel_data_t* data) {
    INFO("Draw pixel")

    int* x;
    int* y;
    size_t size;

    data->smoothing(data->mouse_hist, x, y, &size);
    INFO("Smoothed")

    for (size_t id=0; id<size; id++) {

        if (0 < x[id] && x[id] < data->win->w &&
            0 < y[id] && y[id] < data->win->h ) {
            
            size_t i = x[id];
            size_t j = y[id];
            size_t coord = (j*data->win->w)*4 + 4*i;
            
            data->win->texture[coord  ] = data->color[0];
            data->win->texture[coord+1] = data->color[1];
            data->win->texture[coord+2] = data->color[2];
            data->win->texture[coord+3] = data->color[3];
        } 
    }

    free(x); free(y);
}


void update_texture(app_state_t* app_data) {

    CHECK(WaitForSingleObject(app_data->texture_update_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
    NOTNULL(app_data->window->texture = memcpy(app_data->window->texture, app_data->texture, sizeof(unsigned char)*app_data->w*app_data->h*4),
            "Can't copy from buffer texture to texture")
    CHECK(ReleaseMutex(app_data->texture_update_mtx) != 0, "Failed to release mutex");

}



void stb2bsdl(unsigned char* src, unsigned char* dest, unsigned int w, unsigned int h) {
    for (int l=0; l<h; l++) {
        for (int j=0; j<w; j++) {
                size_t coord = (w*(j))*4+(l)*4;  
                
                dest[coord  ] = src[coord+2];
                dest[coord+1] = src[coord+1];
                dest[coord+2] = src[coord];
                dest[coord+3] = src[coord+3];        
        }   
    }
}


void bsdl2stb(unsigned char* src, unsigned char* dest, unsigned int w, unsigned int h) {
    for (int l=0; h; l++) {
        
        for (int j=0; j<w; j++) {
                size_t coord = (w*(j))*4+(l)*4;  
                
                dest[coord  ] = src[coord+2];
                dest[coord+1] = src[coord+1];
                dest[coord+2] = src[coord];
                dest[coord+3] = src[coord+3];        
        }   
    }
}
