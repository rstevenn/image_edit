#ifdef BSDL_DEBUG
    #define LOGLEVEL 2
    #define LOGTYPE 2
#endif

#include "../logs/log.h"
#include <string.h>
#include "bsdl.h"

// check data
char bsdl_init = 0;
#define BSDL_INIT_CHECK() CHECK(bsdl_init != 0, "bsdl isn't init, use bsdlInit")
#define BSDL_INIT_WINDOW_CHECK(win) CHECK(win->is_init, "the window isn't init")

// utils macros
#define RGBAtoUINT(r, g, b, a) ( ((bsdl_color)b) | ((bsdl_color)g) << 8 |  ((bsdl_color)r) << 16 |  ((bsdl_color)a) << 24)
#define XYtoID(win, x, y) (win->width*y+x)
#define AlphaColorComp(a1, a2, ablend, c1, c2) (( (c1)*(a1) + (c2)*(a2)*(1.0-(a1))) / (ablend))

#define min(a, b) ((a>b) ? (b) : (a))
#define max(a, b) ((a<b) ? (b) : (a))

// math 
inline void cartesian2bary(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y, float* a, float* b, float* c) {
    a[0] = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
    b[0] = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
    c[0] = 1-a[0]-b[0]; 
}

// mem management
void* memset32(void* dest, unsigned int value, size_t count) {

    NOTNULL(dest, "Cant set on a null pointer");

    for (size_t i=0; i<count; i++)
        ((unsigned int*)dest)[i] = value;
    
    return dest;
}

typedef struct _struct_bsdl_color {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char a;
} _bsdl_color;

// draw commands
void bsdlResetWindow(bsdl_window* win, unsigned char  r,unsigned char  g, unsigned char b, unsigned char a) {
    
    BSDL_INIT_WINDOW_CHECK(win)

    bsdl_color color = RGBAtoUINT(r, g, b, a);

    memset32(win->texture, color, win->height*win->width);
    NOTNULL(win->texture, "cant't reset bsdl texture with color: (%x) r: %x g: %x b: %x a: %x",
            color, r, g, b, a)


    INFO("BSDM windows reset with color: (%x) r: %x g: %x b: %x a: %x",
            color, r, g, b, a)

}


void bsdlDrawPixelXY(bsdl_window* win, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode) {
    BSDL_INIT_WINDOW_CHECK(win)

    if (0 <= x && x < win->width) {
        if (0 <= y && y < win->height) {
            switch (mode)
            {
                case BSDL_DRAW_REPLACE : {
                    win->texture[XYtoID(win, x, y)] = RGBAtoUINT(r, g, b, a);
                    break;
                }

                case BSDL_DRAW_ADD : {
                    size_t id = XYtoID(win, x, y);
                    _bsdl_color color = ((_bsdl_color*)win->texture)[id];
                
                    win->texture[id] = RGBAtoUINT(min(r+color.r, 255), min(g+color.g, 255), min(b+color.b, 255), min(a+color.a, 255));

                    break;
                }

                case BSDL_DRAW_MULT : {
                    size_t id = XYtoID(win, x, y);
                    _bsdl_color color = ((_bsdl_color*)win->texture)[id];
                
                    win->texture[id] = RGBAtoUINT((int) (255 * ((float)r/255) * ((float)color.r/255) ), 
                                                  (int) (255 * ((float)g/255) * ((float)color.g/255) ), 
                                                  (int) (255 * ((float)b/255) * ((float)color.b/255) ), 
                                                  (int) (255 * ((float)a/255) * ((float)color.a/255) ));

                    break;
                }

                case BSDL_DRAW_ALPHA : {
                    size_t id = XYtoID(win, x, y);
                    _bsdl_color color = ((_bsdl_color*)win->texture)[id];
                    
                    float a1 = ((float)a) / 255.0;
                    float a2 = ((float)color.a) / 255;
                    float nalpha = a1 + (1.0 - a1) * a2;

                    win->texture[id] = RGBAtoUINT((int) (255 * AlphaColorComp(a1, a2, nalpha, ((float)r) / 255.0, ((float)color.r) / 255.0)), 
                                                  (int) (255 * AlphaColorComp(a1, a2, nalpha, ((float)g) / 255.0, ((float)color.g) / 255.0)), 
                                                  (int) (255 * AlphaColorComp(a1, a2, nalpha, ((float)b) / 255.0, ((float)color.b) / 255.0)), 
                                                  (int) (255 * nalpha));

                    break;
                }

                default:
                    NOT_IMPLEMENTED()
                }
            
        }
    }
    //INFO("BSDM draw pixell with color: (%x) r: %x g: %x b: %x a: %x",
    //       RGBAtoUINT(r, g, b, a), r, g, b, a)
}


void bsdlDrawLineXY(bsdl_window* win, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode) {
    BSDL_INIT_WINDOW_CHECK(win)

    if (x1 == x2 && y1 == y2) {
        bsdlDrawPixelXY(win, x1, y1, r, g, b, a, mode);
    
    } if (abs(x1-x2) > abs(y1-y2)) {
        int step = abs(x1-x2);
        float dx = (float)(x2-x1) / (float)step;
        float dy = (float)(y2-y1) / (float)step;

        for (size_t i=0; i<step+1; i++) {
            bsdlDrawPixelXY(win, (int)(x1+dx*i), (int)(y1+dy*(float)i), r, g, b, a, mode);
        }

    } else {
        int step = abs(y1-y2);
        float dx = (float)(x2-x1) / (float)step;
        float dy = (float)(y2-y1) / (float)step;

        for (size_t i=0; i<step+1; i++) {
            bsdlDrawPixelXY(win, (int)(x1+dx*i), (int)(y1+dy*(float)i), r, g, b, a, mode);
        }
    }

    INFO("BSDM draw line with color: (%x) r: %x g: %x b: %x a: %x",
           RGBAtoUINT(r, g, b, a), r, g, b, a) 

}


void bsdlDrawFilledCricleXY(bsdl_window* win, int x, int y, float radius, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode) {
    BSDL_INIT_WINDOW_CHECK(win)

    for (float dx=-radius; dx<radius+1; dx++) {
        for (float dy=-radius; dy<radius+1; dy++) {
    
            if ((dx*dx + dy*dy) < radius*radius)
                bsdlDrawPixelXY(win, x+(int)dx, y+(int)dy, r, g, b, a, mode);
        }
    }
    
    INFO("BSDM draw circle with color: (%x) r: %x g: %x b: %x a: %x",
           RGBAtoUINT(r, g, b, a), r, g, b, a)

}


void bsdlDrawAllignedFilledRectXY(bsdl_window* win, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char a, bsdl_draw_mode mode) {
    BSDL_INIT_WINDOW_CHECK(win)

    for (int dx=-w/2; dx<w/2+1; dx++) {
        for (int dy=-h/2; dy<h/2+1; dy++) {

            bsdlDrawPixelXY(win, x+dx, y+dy, r, g, b, a, mode);
        }
    }
    
    INFO("BSDM draw rect with color: (%x) r: %x g: %x b: %x a: %x",
           RGBAtoUINT(r, g, b, a), r, g, b, a)    

}

// global
void bsdlInit(void) {
    
    CHECK(SDL_Init(SDL_INIT_EVERYTHING) == 0, "failed to init sdl2");
    bsdl_init = 1;
}

void bsdlFree(void) {

    SDL_Quit();
    bsdl_init = 0;
}


// window
bsdl_window* bsdlInitWindow(size_t w, size_t h, char* name) {
    BSDL_INIT_CHECK() 

    // init bsdl    
    bsdl_window* window = (bsdl_window*)malloc(sizeof(bsdl_window));
    NOTNULL(window, "Unable to allocate a new window");

    window->is_init = 1;

    window->width = w;
    window->height = h;

    // init bsdl texture
    window->texture = (bsdl_color*)malloc(sizeof(bsdl_color)*w*h);
    NOTNULL(window->texture, "Unable to allocate bsdl texture")

    INFO("BSDL window init: OK");

    // init sdl2

    // init sdl2 window
    window->sdl_window = SDL_CreateWindow(name,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    NOTNULL(window->sdl_window, "failed to init sdl2 window")

    // init sdl2 renderer
    window->render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    window->sdl_renderer = SDL_CreateRenderer(window->sdl_window, -1, window->render_flags);
    NOTNULL(window->sdl_renderer, "failed to init sdl2 renderer")

    // init sdl2 texture
    window->sdl_texture = SDL_CreateTexture(window->sdl_renderer, 
                                            SDL_PIXELFORMAT_ARGB8888, 
                                            SDL_TEXTUREACCESS_STREAMING, w, h);
    NOTNULL(window->sdl_texture, "failed to init sdl2 texture")

    INFO("SDL2 init: OK")

    return window;
}


void bsdlUpdateWindow(bsdl_window* win, void (*eventHendler)(SDL_Event, bsdl_window* win, void*), void* callBackData) {
    
    BSDL_INIT_CHECK() 
    BSDL_INIT_WINDOW_CHECK(win)
    
    // event loop
    SDL_Event event;
    while (SDL_PollEvent(&event)) {    
        // use callback functions
        if (eventHendler != NULL) {
            INFO("called back: event type:%lu", event.type)
            (eventHendler)(event, win, callBackData);
        }

        
        switch (event.type) {

            case SDL_WINDOWEVENT:
            {
                break;
            }
            
            default:
                break;
        }
    }

    INFO("SDL2 event Loop: OK")

    // copy texture
    SDL_UpdateTexture(win->sdl_texture, NULL, win->texture,
                      sizeof(bsdl_color)*win->width);

    // render sdl2
    SDL_RenderClear(win->sdl_renderer);

    CHECK(SDL_RenderCopy(win->sdl_renderer, win->sdl_texture, NULL, NULL) == 0, "Can't render texture");

    SDL_SetRenderDrawColor(win->sdl_renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderPresent(win->sdl_renderer);

    INFO("SLD2 render: OK")

}


void bsdlFreeWindow(bsdl_window* win) {

    if (win->is_init == 0)
        WARNING("freeing an uninitialized window")

    // free sdl2
    SDL_DestroyTexture(win->sdl_texture);
    SDL_DestroyRenderer(win->sdl_renderer);
    SDL_DestroyWindow(win->sdl_window);

    INFO("SDL2 free: OK");

    // free bsdl
    win->is_init = 0;
    free(win->texture);
    free(win);
    INFO("BSDL window free: OK");
}