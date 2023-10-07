#ifndef __BACK_END_H__
#define __BACK_END_H__


#include <stdio.h>
#include <stdlib.h>

#include "../shared_def.h"
#undef ERROR
#include <windows.h>
#undef ERROR

#include "../../config.h"
#include "../../ccbase/logs/log.h"
#include "../../ccbase/betterSDL/bsdl.h"
#include "hist.h"



// communication with front end
size_t pull_window_event(app_state_t*, call_back_t*);
void update_texture(app_state_t* window_data);
void callback_handler(call_back_t);

// main texture gestion
void stb2bsdl(unsigned char* src, unsigned char* dest, unsigned int w, unsigned int h); 
void bsdl2stb(unsigned char* src, unsigned char* dest, unsigned int w, unsigned int h); 

// update texture
void cache_update_pxl(unsigned char* texture, pxl_change_cache* cache, size_t id, unsigned char color[4]);
void cache_revert(unsigned char* texture, pxl_change_cache* cache);

#endif