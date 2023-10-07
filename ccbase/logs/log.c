#include "log.h"

// global var about the log file
static FILE* outLog;
static int init=0;


// Init the log file
void ccb_InitLog(const char* path) 
{
    if (init == 1)
        {CCB_PERROR("LOGFILE ALLREADY INIT"); exit(-1);}

    outLog = fopen(path, "w");
    init = 1;
    CCB_PNOTNULL(outLog, "Can't open logfile:%s", path)
}

// Get the log file pointer
FILE* ccb_GetLogFile() 
{
    if (init) {
        return outLog;
    } else 
        {CCB_PERROR("The logfile must be init by using InitLog"); exit(-1);}
}

// Close the log file
void ccb_CloseLogFile() 
{
    if (init) {
        fclose(outLog);
        init = 0;
    } else 
        {CCB_PERROR("The logfile must be init by using InitLog"); exit(-1);}
}