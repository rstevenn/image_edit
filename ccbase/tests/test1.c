#include <stdio.h>
#include <time.h>

#define LOGLEVEL 2
#define LOGTYPE 2

#include "../logs/log.h"
#include "../betterSDL/bsdl.h"

int main(int argc, char *argv[]) {

    ccb_InitLog("log.txt");
 
    CCB_INFO("Test 1")
    
    bsdlInit();

    bsdl_window* window  = bsdlInitWindow(2048, 1024, "test1");
    bsdl_window* window2 = bsdlInitWindow(1024, 1024, "test2");
    
    size_t frame = 0;
    time_t seconds;
    seconds = time(NULL);
    

    for (size_t i=0; i<1000; i++) { 
        bsdlResetWindow(window2, 5, 5, 16, 255);
        
        bsdlDrawAllignedFilledRectXY(window2, 1024, 1024, 250, 150, 255, 255, 255, 255, BSDL_DRAW_REPLACE);

        bsdlDrawAllignedFilledRectXY(window2, 512, 512, 250, 150, 255, 255, 255, 255, BSDL_DRAW_REPLACE);
        bsdlDrawFilledCricleXY(window2, 500 + 24*((float)i/999), 512, 50, 255, 0, 0, 255, BSDL_DRAW_MULT);

        bsdlDrawLineXY(window2, 387, 437, 637, 587, 0, 0, 255, 255, BSDL_DRAW_MULT);
        bsdlDrawLineXY(window2, 387, 587, 637, 437, 0, 0, 255, 255, BSDL_DRAW_ADD);

        bsdlDrawAllignedFilledRectXY(window2, 512, 512, 400, 400, 128, 255, 128, 128, BSDL_DRAW_ALPHA);

        bsdlUpdateWindow(window, NULL, NULL);
        bsdlUpdateWindow(window2, NULL, NULL);
        
        frame++;
        if (time(NULL) > seconds) {
            
            CCB_INFO(" FPS: %lu", frame)

            frame = 0;
            seconds = time(NULL);
        }
    }

    bsdlFreeWindow(window);

    for (size_t i=0; i<1000; i++) { 
        
        bsdlUpdateWindow(window2, NULL, NULL);
        SDL_Delay(1);
    }
    
    bsdlFreeWindow(window2);
    bsdlFree();

    ccb_CloseLogFile();
 
    return 0;
}