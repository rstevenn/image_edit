#include "log.h"

// global var about the log file
static FILE* outLog;
static int init=0;


// Init the log file
void InitLog(const char* path) 
{
    if (init == 1)
        {PERROR("LOGFILE ALLREADY INIT"); exit(-1);}

    outLog = fopen(path, "w");
    init = 1;
    PNOTNULL(outLog, "Can't open logfile:%s", path)
}

// Get the log file pointer
FILE* GetLogFile() 
{
    if (init) {
        return outLog;
    } else 
        {PERROR("The logfile must be init by using InitLog"); exit(-1);}
}

// Close the log file
void CloseLogFile() 
{
    if (init) {
        fclose(outLog);
        init = 0;
    } else 
        {PERROR("The logfile must be init by using InitLog"); exit(-1);}
}