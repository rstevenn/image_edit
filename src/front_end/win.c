#include "win.h"

#define IS_INIT() CHECK(window_is_init, "Init the window using InitWindow before calling this function")
#define ISNT_INIT() CHECK(!window_is_init, "InitWindow allready called")

unsigned char window_is_init = 0;


app_state_t* InitWindow(size_t w, size_t h, unsigned char* texture){
    INFO("Init Window")
    window_is_init = 1;

    app_state_t* app_data = (app_state_t *)malloc(sizeof(app_state_t));
    app_data->h = w;
    app_data->w = h;
    app_data->texture = texture;
    app_data->run = 0;
    app_data->event_nb = 0;
    app_data->call_backs = NULL;
    app_data->window = NULL;

    app_data->texture_update_mtx = CreateMutexW(NULL, FALSE, NULL);
    app_data->event_acces_mtx = CreateMutexW(NULL, FALSE, NULL);

    return app_data;

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

    app_state_t* app_data = (app_state_t*)args;
    call_back_t* queue;

    INFO("Start BSDL")
    bsdlInit();
    bsdl_window* window  = bsdlInitWindow(512, 512, "ImageEdit");
    app_data->window = window;

    mouse_state_t mouse_state = {0};
    app_data->run = 1;

    while (! mouse_state.quit) {
        
        CHECK(WaitForSingleObject(app_data->texture_update_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
        bsdlUpdateWindow(window, window_event_handler, (void*)&mouse_state);
        CHECK(ReleaseMutex(app_data->texture_update_mtx) != 0, "Failed to release mutex");
        
        
        if (mouse_state.left) {
            INFO("clicked")
            CHECK(WaitForSingleObject(app_data->event_acces_mtx, WAIT_TIMEOUT) == 0, "wait mutex failed")
            INFO("Acces mutex")
            

            queue = (call_back_t *)(app_data->call_backs);
            NOTNULL((queue = realloc(queue, (app_data->event_nb+1) * sizeof(call_back_t))), "Can't realloc the event queue")
    
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
            cb_data.win = app_data;

            queue[app_data->event_nb].data.draw_pixel_data = cb_data;
            queue[app_data->event_nb].function.draw_pixel = &draw_pxl;
        
            app_data->event_nb += 1;
            app_data->call_backs = queue;
            INFO("Queue added, %lu elements", app_data->event_nb)

            
            CHECK(ReleaseMutex(app_data->event_acces_mtx) != 0, "Failed to release mutex");
            INFO("Mutex released")

        }
        SDL_Delay(1);
        
    }

    INFO("Unload BSDL")
    bsdlFreeWindow(window);
    bsdlFree();

    app_data->run = 0;
};

