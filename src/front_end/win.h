#ifndef __WIN_H__
#define __WIN_H__


#include <stdio.h>
#include <stdlib.h>

#include "../shared_def.h"
#include <windows.h>


#include "../../config.h"
#include "../../ccbase/logs/log.h"
#include "../../ccbase/betterSDL/bsdl.h"


// functions

app_state_t* InitWindow(size_t w, size_t h, unsigned char* texture);
void RelaseWindow(app_state_t*);

void window_event_handler(SDL_Event event, bsdl_window* win, void* arg);
void window_th(void* args);


#endif