#ifndef __BACK_END_H__
#define __BACK_END_H__


#include <stdio.h>
#include <stdlib.h>

#include "../shared_def.h"
#undef ERROR
#include <windows.h>
#undef ERROR

#define LOGLEVEL 2
#define LOGTYPE 2

#include "../../ccbase/logs/log.h"
#include "../../ccbase/betterSDL/bsdl.h"


// communication with front end
size_t pull_window_event(app_state_t*, call_back_t*);
void callback_handler(call_back_t);

// main texture gestion
void stb2bsdl(unsigned char* src, unsigned char* dest, unsigned int w, unsigned int h); 
void bsdl2stb(unsigned char* src, unsigned char* dest, unsigned int w, unsigned int h); 

#endif