#include <stdio.h>
#include <stdlib.h>

// basic log script (NOT THREAD SAFE)

// init create and delete logFile
// Use a singleton like pointer shared throug all the project

void InitLog(const char* path); 
/**
 * @brief Init the globaly shared log file pointer (one init by project)
 * 
 * @param const char* path: the path to the log file 
 * 
 * @author Romain Stévenne
 * @since 15-11-2022
**/

FILE* GetLogFile();
/**
 * @brief Get the globaly shared log file pointer
 * 
 * @return FILE* logFile: return the log file pointer
 * 
 * @author Romain Stévenne
 * @since 15-11-2022
**/

void CloseLogFile();
/**
 * @brief Close the globaly shared log file pointer (one close by project)
 * 
 * @author Romain Stévenne
 * @since 15-11-2022
**/


/*
    MACROS USE TO PRINT LOGS INSIDE A FILE OR ON THE COMMAND PROMPT
    
    default version controled by the LOGTYPE macro
    P version print on the command prompt
    F version print on the file define with void InitLog(const char* path); 

    The macros must be define before including this file
    The value of these macros can be changed in each file independently

    LOGLEVEL:
        LOGLEVEL = 0 or undifiend: Only shows ERRORS
        LOGLEVEL = 1: Show ERRORS and WARNINGS
        LOGLEVEL = 2: Show ERRORS WARNINGS INFO and NOTIMPLEMENTED

    LOGTYPE:
        LOGTYPE = 0 or undifiend: print logs in the command prompt
        LOGTYPE = 1: print logs in the log file
        LOGTYPE = 2: print logs in the command prompt and log file
*/


// ERRORS
#define FERROR(...) { fprintf(GetLogFile(), "[     ERROR     ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                          fprintf(GetLogFile(), __VA_ARGS__);}
#define PERROR(...) { printf("[     ERROR     ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                           printf(__VA_ARGS__); printf("\n");}

// WARNINGS
#if LOGLEVEL >= 1
    #define FWARNING(...) { fprintf(GetLogFile(), "[    WARNING    ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                            fprintf(GetLogFile(), __VA_ARGS__); fprintf(GetLogFile(), "\n");} 
    #define PWARNING(...) { printf("[    WARNING    ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                            printf(__VA_ARGS__); printf("\n"); }
#else
    #define FWARNING(...)
    #define PWARNING(...)
#endif


// INFOS & NOTIMP
#if LOGLEVEL >= 2
    #define FINFO(...) { fprintf(GetLogFile(), "[     INFO      ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                         fprintf(GetLogFile(), __VA_ARGS__); fprintf(GetLogFile(), "\n");}
    #define PINFO(...) { printf("[     INFO      ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                         printf(__VA_ARGS__); printf("\n"); }

    #define PNOT_IMPLEMENTED() {fprintf(GetLogFile(), "[NOT IMPLEMENTED] %s::%s()::l%d\n", __FILE__, __FUNCTION__, __LINE__);}
    #define FNOT_IMPLEMENTED() {printf("[NOT IMPLEMENTED] %s::%s()::l%d\n", __FILE__, __FUNCTION__, __LINE__);}

#else
    #define FINFO(...)
    #define PINFO(...)
    #define PNOT_IMPLEMENTED()
    #define FNOT_IMPLEMENTED()
#endif


// NOT NULL
#define FNOTNULL(ptr, ...) if ((ptr) == NULL) ERROR(__VA_ARGS__)
#define PNOTNULL(ptr, ...) if ((ptr) == NULL) ERROR(__VA_ARGS__)
#define FCHECK(cond, ...) if (!(cond)) ERROR(__VA_ARGS__)
#define PCHECK(cond, ...) if (!(cond)) ERROR(__VA_ARGS__)

// DEFAULT VERSIONS
#if LOGTYPE == 1
    #define ERROR(...) {FERROR(__VA_ARGS__); exit(-1);}
    #define WARNING(...) FWARNING(__VA_ARGS__)
    #define INFO(...) FINFO(__VA_ARGS__)
    #define NOT_IMPLEMENTED() FNOT_IMPLEMENTED()
    #define NOTNULL(ptr, ...) FNOTNULL(ptr, __VA_ARGS__)
    #define CHECK(cond, ...) FCHECK(cond, __VA_ARGS__)

#else
#if LOGTYPE == 2
    #define ERROR(...) {PERROR(__VA_ARGS__) FERROR(__VA_ARGS__) exit(-1); }
    #define WARNING(...) PWARNING(__VA_ARGS__) FWARNING(__VA_ARGS__)
    #define INFO(...) PINFO(__VA_ARGS__) FINFO(__VA_ARGS__)
    #define NOT_IMPLEMENTED() PNOT_IMPLEMENTED() FNOT_IMPLEMENTED()
    #define NOTNULL(ptr, ...) PNOTNULL(ptr, __VA_ARGS__) FNOTNULL(ptr, __VA_ARGS__)
    #define CHECK(cond, ...) PCHECK(cond, __VA_ARGS__) FCHECK(cond, __VA_ARGS__)

#else
    #define ERROR(...) {PERROR(__VA_ARGS__) exit(-1);}
    #define WARNING(...) PWARNING(__VA_ARGS__)
    #define INFO(...) PINFO(__VA_ARGS__)
    #define NOT_IMPLEMENTED() PNOT_IMPLEMENTED()
    #define NOTNULL(ptr, ...) PNOTNULL(ptr, __VA_ARGS__)
    #define CHECK(cond, ...) PCHECK(cond, __VA_ARGS__)

#endif
#endif