#ifndef __SHARED_DEF_H__
#define __SHARED_DEF_H__

#include <windows.h>
#undef ERROR

#include "../ccbase/betterSDL/bsdl.h"


// main data of the app
typedef struct {
    unsigned char* texture;
    size_t w;
    size_t h;

    unsigned run;
    bsdl_window* window;

    HANDLE event_acces_mtx;
    HANDLE texture_update_mtx;

    size_t event_nb;
    void* call_backs;
} app_state_t;


typedef struct {
    int x[3];
    int y[3];

    int next_x;
    int next_y;

    size_t prev_size;
    unsigned char next;

} mouse_pos_history_t;


typedef struct {
    unsigned char left;
    unsigned char right;
    
    int x, y;
    unsigned char quit;

    float u, v;
} mouse_state_t;


// smooth data
typedef struct {
    size_t size;
    int* x;
    int* y;
} smoothed_path_t;


// call backs 
typedef void(smoothing_t)(mouse_pos_history_t state, smoothed_path_t* smoothed_path); 

typedef struct {
    mouse_state_t mouse;
    smoothing_t* smoothing;
    
    unsigned char color[4];
    app_state_t* win;

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

// functions for callbak
void draw_pxl(draw_pixel_data_t* data);
void four_points_smoothing(mouse_pos_history_t state, smoothed_path_t* smoothed_path); // ALLOCATE DATA ON X Y 


#endif 