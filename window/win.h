#ifndef __WIN_H__
#define __WIN_H__


#include <stdio.h>
#include <stdlib.h>

#undef ERROR
#include <windows.h>
#undef ERROR

#define LOGLEVEL 2
#define LOGTYPE 2

#include "../ccbase/logs/log.h"
#include "../ccbase/betterSDL/bsdl.h"


typedef struct {
    unsigned char* texture;
    size_t w;
    size_t h;

    unsigned run;
    bsdl_window* window;

    size_t event_nb;
    void* call_backs;
} window_data_t;


typedef struct {
    unsigned char left;
    unsigned char right;
    int x, y;
    unsigned char quit;
} mouse_state_t;


// API
typedef struct {
    float u, v;
    unsigned char color[4];
    window_data_t* win;

} draw_pixel_data_t;

typedef void(draw_pixel_t)(draw_pixel_data_t*); 


// callback data
typedef union {
    draw_pixel_t* draw_pixel;

} call_back_fnc_t;


typedef union {
    void* none;
    draw_pixel_data_t draw_pixel_data;
} call_back_data_t;


typedef enum {
    CB_DRAW_PIXEL
} call_back_types;


typedef struct {

    call_back_fnc_t function;
    call_back_data_t data;
    call_back_types type;

} call_back_t;


// functions


window_data_t* InitWindow(size_t w, size_t h, unsigned char* texture);
void RelaseWindow(window_data_t*);

void window_event_handler(SDL_Event event, bsdl_window* win, void* arg);
void window_th(void* args);
size_t pull_window_event(window_data_t*, call_back_t*);

void callback_handler(call_back_t);
void update_texture(window_data_t* window_data);

#endif