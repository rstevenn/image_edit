#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#ifndef __BSDL_H__
#define __BSDL_H__

#include "../logs/log.h"

typedef unsigned int bsdl_color;

typedef enum _bsdl_draw_mode {
    BSDL_DRAW_REPLACE,
    BSDL_DRAW_ADD,
    BSDL_DRAW_MULT,
    BSDL_DRAW_ALPHA

} bsdl_draw_mode;

typedef struct _bsdl_window {
    
    // meta
    char is_init : 1;
    Uint32 render_flags;

    // base
    size_t width;
    size_t height;

    bsdl_color* texture;

    // SDL
    SDL_Window* sdl_window;
  	SDL_Renderer* sdl_renderer;
    SDL_Texture *sdl_texture;

} bsdl_window;


// draw commands
void bsdlResetWindow(bsdl_window* win, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

void bsdlDrawPixelXY(bsdl_window* win, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode);
void bsdlDrawLineXY(bsdl_window* win, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode);

void bsdlDrawFilledCricleXY(bsdl_window* win, int x, int y, float radius, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode);
void bsdlDrawAllignedFilledRectXY(bsdl_window* win, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode);


// global
void bsdlInit(void);
void bsdlFree(void);


// window
bsdl_window* bsdlInitWindow(size_t w, size_t h, char* name);
void bsdlUpdateWindow(bsdl_window* win, void (*eventHendler)(SDL_Event, bsdl_window* win, void* callBackData), void* callBackData); 
void bsdlFreeWindow(bsdl_window* win);

#endif