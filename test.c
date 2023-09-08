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

#include "src/front_end/win.h"
#include "src/back_end/server.h"


void test_smoothing(void) {
    INFO("TEST SMOOTHING")

    mouse_pos_history_t hist;
    hist.prev_size = 3;

    hist.x[0] = 0; hist.x[1] = 1; hist.x[2] = 3;
    hist.y[0] = 0; hist.y[1] = 1; hist.y[2] = 3;


    smoothed_path_t smoothed_path;
    cubic_smoothing(hist, &smoothed_path);

    INFO("SIZE : %lu", smoothed_path.size)
    INFO("POS  : %d %d", smoothed_path.x[0], smoothed_path.y[0])


    INFO("END SMOOTHING")
}



int main (int argc, char* argv[]) {

    InitLog("log.txt");
    INFO("Start Log")

    test_smoothing();

    INFO("Unload Log")
    CloseLogFile();

    return 1;
}