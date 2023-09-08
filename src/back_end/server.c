#include "server.h"
#include <math.h>

float length(float dx, float dy) {
    return sqrtf((dx)*(dx) + (dy)*(dy));
}

void cubic_smoothing(mouse_pos_history_t state, smoothed_path_t* smoothed_path) {


    float alpha = 0.9;

    if (state.prev_size == 1) {
        INFO("SMOOTH 1")
        
        
        float px = (float)state.x[0];
        float py = (float)state.y[0];
        INFO("GOT POS")

        // push init pos
        NOTNULL(smoothed_path->x = (int*)malloc(sizeof(int)), "can't allocate memory")
        NOTNULL(smoothed_path->y = (int*)malloc(sizeof(int)), "can't allocate memory")
        smoothed_path->x[0] = px; smoothed_path->y[0] = py;

        smoothed_path->size = 1;
        INFO("SMOOTH 1 END")

    } else if (state.prev_size == 2) {

        
        float px = (float)state.x[0];
        float py = (float)state.y[0];

        // push init pos
        NOTNULL(smoothed_path->x = (int*)malloc(sizeof(int)), "can't allocate memory")
        NOTNULL(smoothed_path->y = (int*)malloc(sizeof(int)), "can't allocate memory")
        smoothed_path->x[0] = px; smoothed_path->y[0] = py;

        smoothed_path->size = 1;

    } else {
        INFO("SMOOTH >2")
        float dxa = (float)state.x[1] - (float)state.x[2]; 
        float dya = (float)state.y[1] - (float)state.y[2];

        float la = length(dxa, dya);

        float vx, vy;

        if (la > 0) {
            vx = dxa / la;
            vy = dya / la;
        } else {
            vx = 0;
            vy = 0;   
        }

        size_t i=1;
        
        float px = (float)state.x[1];
        float py = (float)state.y[1];

        // push init pos
        NOTNULL(smoothed_path->x = (int*)malloc(sizeof(int)), "can't allocate memory")
        NOTNULL(smoothed_path->y = (int*)malloc(sizeof(int)), "can't allocate memory")
        smoothed_path->x[0] = px; smoothed_path->y[0] = py;

        INFO("pos: %f %f", px, py)
        INFO("target pos: %f %f", (float)state.x[0], (float)state.y[0])
        INFO("LOOP STATUTS: %lu", px != (float)state.x[0] || py != (float)state.y[0])

        while (px != state.x[0] ||
               py != state.y[0]) {
            INFO("LOOP")
            //INFO("Dist: %f", length( px-(float)state.x[0], py-(float)state.y[0]))
            //INFO("speed: %f %f", vx, vy)

            px += vx;
            py += vy; 

            float target_vx = (float)state.x[0] - px;
            float target_vy = (float)state.y[0] - py;

            if (length(target_vx, target_vy) > 1) {
                vx = vx * alpha + (1-alpha) * target_vx / length(target_vx, target_vy);
                vy = vy * alpha + (1-alpha) * target_vy / length(target_vx, target_vy);
            } else {
                vx = vx * alpha + (1-alpha) * target_vx;
                vy = vy * alpha + (1-alpha) * target_vy;
            }
            
            alpha = alpha*alpha;

            i++;
            NOTNULL(smoothed_path->x = (int*)realloc(smoothed_path->x, sizeof(int)*i), "can't allocate memory")
            NOTNULL(smoothed_path->y = (int*)realloc(smoothed_path->y, sizeof(int)*i), "can't allocate memory")

            smoothed_path->x[i-1] = px;
            smoothed_path->y[i-1] = py;
            smoothed_path->size = i;
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

    smoothed_path_t smoothed_path = {0};
    (*data->smoothing)(data->mouse_hist, &smoothed_path);
    INFO("Smoothed")

    for (size_t id=0; id<smoothed_path.size; id++) {

        if (0 < smoothed_path.x[id] && smoothed_path.x[id] < data->win->w &&
            0 < smoothed_path.y[id] && smoothed_path.y[id] < data->win->h ) {

            INFO("DRAW %d %d", smoothed_path.x[id], smoothed_path.y[id])
            size_t i = smoothed_path.x[id];
            size_t j = smoothed_path.y[id];
            size_t coord = (j*data->win->w)*4 + 4*i;
            
            data->win->texture[coord  ] = data->color[0];
            data->win->texture[coord+1] = data->color[1];
            data->win->texture[coord+2] = data->color[2];
            data->win->texture[coord+3] = data->color[3];
        } 
    }

    free(smoothed_path.x); free(smoothed_path.y);
    INFO("end draw pxl")
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
