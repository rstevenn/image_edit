#include "win.h"

#define IS_INIT() CHECK(window_is_init, "Init the window using InitWindow before calling this function")
#define ISNT_INIT() CHECK(!window_is_init, "InitWindow allready called")

//HANDLE win_texture_update_mtx = NULL;
unsigned char window_is_init = 0;


app_state_t* InitWindow(size_t w, size_t h, unsigned char* texture){
    INFO("Init Window")
    window_is_init = 1;

    //win_texture_update_mtx = CreateMutexW(NULL, FALSE, NULL);

    app_state_t* window_data = (app_state_t *)malloc(sizeof(app_state_t));
    window_data->h = w;
    window_data->w = h;
    window_data->texture = texture;
    window_data->run = 1;
    window_data->event_nb = 0;
    window_data->call_backs = NULL;
    window_data->window = NULL;
    window_data->win_event_accest_mtx = CreateMutexW(NULL, FALSE, NULL);

    return window_data;

}


void RelaseWindow(app_state_t* data) {
    IS_INIT()
    INFO("Free window")
    
    free(data);
    window_is_init = 0;
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

    app_state_t* win_data = (app_state_t*)args;
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
            CHECK(WaitForSingleObject(win_data->win_event_accest_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
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

            
            CHECK(ReleaseMutex(win_data->win_event_accest_mtx) != 0, "Failed to release mutex");
            INFO("Mutex released")

        }
        SDL_Delay(10);
        
    }

    INFO("Unload BSDL")
    bsdlFreeWindow(window);
    bsdlFree();

    win_data->run = 0;
};


void update_texture(app_state_t* window_data) {

    
    NOTNULL(window_data->window->texture = memcpy(window_data->window->texture, window_data->texture, sizeof(unsigned char)*window_data->w*window_data->h*4),
            "Can't copy from buffer texture to texture")


}

