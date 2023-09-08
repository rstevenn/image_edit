#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


#undef ERROR
#include "config.h"
#include "ccbase/logs/log.h"
#include "ccbase/betterSDL/bsdl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "src/shared_def.h"
#include "src/front_end/win.h"
#include "src/back_end/server.h"



int main (int argc, char* argv[]) {

    InitLog("log.txt");
    INFO("Start Log")

    int x, y, comp, res;
    unsigned char *data;
    unsigned char *image;

    res = stbi_info(INPUT_FILE, &x, &y, &comp);
    INFO("File data %s with width=%d height=%d comp=%d res=%d", INPUT_FILE, x, y, comp, res);

    data = stbi_load(INPUT_FILE, &x, &y, &comp, 4);
    NOTNULL(image = (unsigned char *)malloc(sizeof(unsigned char) *x*y*4), "Can't alloc image buffer")
    stb2bsdl(data, image, x, y);

    INFO("File data loaded %s with width=%d height=%d comp=%d", INPUT_FILE, x, y, comp);
    if (data == NULL) { printf("Failed\n"); exit(1);}

    app_state_t* app_data = InitWindow(x, y, image);    
    
    (HANDLE)_beginthread(window_th, 0, app_data);

    do {
        SDL_Delay(1);
    } while (! (app_data->run));
    

    while (app_data->run) {
        call_back_t call_back;
        INFO("Gota get those event")

        while (pull_window_event(app_data, &call_back) > 0)
        {
            INFO("called Back")
            callback_handler(call_back);
        }

        update_texture(app_data);
        SDL_Delay(1);
        
    }

    INFO("Save Image");
    bsdl2stb(image, data, x, y);
    stbi_write_png(OUTPUT_FILE, x, y, 4, data, 0);
    free(data);

    RelaseWindow(app_data);

    INFO("Unload Log")
    CloseLogFile();

    return 1;
}


