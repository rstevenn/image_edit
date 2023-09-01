#include "win.h"

#define IS_INIT() CHECK(window_is_init, "Init the window using InitWindow before calling this function")
#define ISNT_INIT() CHECK(!window_is_init, "InitWindow allready called")

HANDLE win_event_accest_mtx = NULL;
//HANDLE win_texture_update_mtx = NULL;
unsigned char window_is_init = 0;


window_data_t* InitWindow(size_t w, size_t h, unsigned char* texture){
    INFO("Init Window")
    window_is_init = 1;

    win_event_accest_mtx   = CreateMutexW(NULL, FALSE, NULL);
    //win_texture_update_mtx = CreateMutexW(NULL, FALSE, NULL);

    window_data_t* window_data = (window_data_t *)malloc(sizeof(window_data_t));
    window_data->h = w;
    window_data->w = h;
    window_data->texture = texture;
    window_data->run = 1;
    window_data->event_nb = 0;
    window_data->call_backs = NULL;
    window_data->window = NULL;

    return window_data;

}

void RelaseWindow(window_data_t* data) {
    IS_INIT()
    INFO("Free window")
    
    free(data);
    window_is_init = 0;
}


void draw_pxl(draw_pixel_data_t* data) {
    INFO("Draw pixel")
    if (0 < data->u && data->u < 1 &&
        0 < data->v && data->v < 1 ) {

            size_t i = data->u*data->win->w;
            size_t j = data->v*data->win->h;
            size_t coord = (j*data->win->w)*4 + 4*i;

            data->win->texture[coord  ] = data->color[0];
            data->win->texture[coord+1] = data->color[1];
            data->win->texture[coord+2] = data->color[2];
            data->win->texture[coord+3] = data->color[3];
    }

}

void window_event_handler(SDL_Event event, bsdl_window* win, void* arg) {

        INFO("sdl event callback")
        mouse_state_t* mouse_state = (mouse_state_t*)arg;
        INFO("sdl event callback event type %lu, %lu", event.type, SDL_MOUSEBUTTONDOWN)

        switch(event.type){

            case SDL_QUIT:
                INFO("Quit");
                mouse_state->quit = 1;
                break;

            case SDL_MOUSEBUTTONDOWN:
                
                INFO("Click, is left: %d", event.button.button == SDL_BUTTON_LEFT)
                if(event.button.button == SDL_BUTTON_LEFT){
                    mouse_state->left = 1;
                }

                break;

            case SDL_MOUSEBUTTONUP:

                INFO("UnClick")
                if(event.button.button == SDL_BUTTON_LEFT){
                    mouse_state->left = 0;
                }

                break;
        }

        SDL_GetMouseState(&(mouse_state->x), &(mouse_state->y));
}


void window_th(void* args) {
    IS_INIT()

    window_data_t* win_data = (window_data_t*)args;
    call_back_t* queue;

    INFO("Start BSDL")
    bsdlInit();
    bsdl_window* window  = bsdlInitWindow(512, 512, "ImageEdit");
    win_data->window = window;

    mouse_state_t mouse_state = {0};

    while (! mouse_state.quit) {
        update_texture(win_data);
        bsdlUpdateWindow(window, window_event_handler, (void*)&mouse_state);
        
        
        if (mouse_state.left) {
            INFO("clicked")
            CHECK(WaitForSingleObject(win_event_accest_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
            INFO("Acces mutex")
            

            queue = (call_back_t *)(win_data->call_backs);
            NOTNULL((queue = realloc(queue, (win_data->event_nb+1) * sizeof(call_back_t))), "Can't realloc the event queue")
    
            draw_pixel_data_t cb_data;

            int w, h;
            SDL_GetWindowSize(window->sdl_window, &w, &h);

            cb_data.color[0] = 0;
            cb_data.color[1] = 0;
            cb_data.color[2] = 0;
            cb_data.color[3] = 255;
            
            if (w == 0) w = 1;
            if (h == 0) h = 1;

            cb_data.u = ((float)mouse_state.x) / (float)w;
            cb_data.v = ((float)mouse_state.y) / (float)h;
            cb_data.win = win_data;

            queue[win_data->event_nb].data.draw_pixel_data = cb_data;
            queue[win_data->event_nb].function.draw_pixel = &draw_pxl;
        
            win_data->event_nb += 1;
            win_data->call_backs = queue;
            INFO("Queue added, %lu elements", win_data->event_nb)

            
            CHECK(ReleaseMutex(win_event_accest_mtx) != 0, "Failed to release mutex");
            INFO("Mutex released")

        }
        SDL_Delay(10);
        
    }

    INFO("Unload BSDL")
    bsdlFreeWindow(window);
    bsdlFree();

    win_data->run = 0;
};


size_t pull_window_event(window_data_t* win_data, call_back_t* call_back) {
    IS_INIT()

    if (win_data->event_nb == 0) { INFO("No event") return 0;} 

    // wait mutex
    CHECK(WaitForSingleObject(win_event_accest_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
    INFO("got mutex")

    call_back_t* queue = (call_back_t *)(win_data->call_backs);

    call_back[0] = queue[0];     
    
    win_data->event_nb--;
    if (win_data->event_nb > 0) {
        NOTNULL( (memmove(queue, queue + 1, sizeof(call_back_t)*(win_data->event_nb))), "Can't move callbacks to the front of the event queue")
        NOTNULL( (queue = (call_back_t *)realloc(queue, sizeof(call_back_t)*(win_data->event_nb))), "Can't realloc the event queue")
    } else {
        free(queue);
        queue = NULL;
    }

    win_data->call_backs = (void*)queue;
    size_t out = win_data->event_nb;

    // release
    CHECK(ReleaseMutex(win_event_accest_mtx) != 0, "Failed to release mutex");
    INFO("Mutex released")
    return out+1;

}

void callback_handler(call_back_t cb) {

    INFO("call_back start")
    switch (cb.type)
    {
        case CB_DRAW_PIXEL:
            
            (*cb.function.draw_pixel)(&cb.data.draw_pixel_data);
            //free(cb.data.draw_pixel_data);
            break;
    
        default:
            break;
    }
    INFO("call_back end")
    
}

void update_texture(window_data_t* window_data) {
    
    for (int l=0; l<window_data->h; l++) {
        
        for (int j=0; j<window_data->w; j++) {
                size_t coord = (window_data->w*(j))*4+(l)*4;  
                
                ((unsigned char *)window_data->window->texture)[coord  ] = window_data->texture[coord+2];
                ((unsigned char *)window_data->window->texture)[coord+1] = window_data->texture[coord+1];
                ((unsigned char *)window_data->window->texture)[coord+2] = window_data->texture[coord];
                ((unsigned char *)window_data->window->texture)[coord+3] = window_data->texture[coord+3];       
                
        }   
    }
    
}