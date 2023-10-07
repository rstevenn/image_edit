#include <stdio.h>
#include <stdlib.h>

// basic log script (NOT THREAD SAFE)

// init create and delete logFile
// Use a singleton like pointer shared throug all the project

void ccb_InitLog(const char* path); 
/**
 * @brief Init the globaly shared log file pointer (one init by project)
 * 
 * @param const char* path: the path to the log file 
 * 
 * @author Romain Stévenne
 * @since 15-11-2022
**/

FILE* ccb_GetLogFile();
/**
 * @brief Get the globaly shared log file pointer
 * 
 * @return FILE* logFile: return the log file pointer
 * 
 * @author Romain Stévenne
 * @since 15-11-2022
**/

void ccb_CloseLogFile();
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

    CCB_LOGLEVEL:
        CCB_LOGLEVEL = 0 or undifiend: Only shows ERRORS
        CCB_LOGLEVEL = 1: Show ERRORS and WARNINGS
        CCB_LOGLEVEL = 2: Show ERRORS WARNINGS INFO and NOTIMPLEMENTED

    CCB_LOGTYPE:
        CCB_LOGTYPE = 0 or undifiend: print logs in the command prompt
        CCB_LOGTYPE = 1: print logs in the log file
        CCB_LOGTYPE = 2: print logs in the command prompt and log file
*/


// ERRORS
#define CCB_FERROR(...) { fprintf(ccb_GetLogFile(), "[     ERROR     ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                          fprintf(ccb_GetLogFile(), __VA_ARGS__);}
#define CCB_PERROR(...) { printf("[     ERROR     ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                           printf(__VA_ARGS__); printf("\n");}

// WARNINGS
#if CCB_LOGLEVEL >= 1
    #define CCB_FWARNING(...) { fprintf(ccb_GetLogFile(), "[    WARNING    ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                            fprintf(ccb_GetLogFile(), __VA_ARGS__); fprintf(ccb_GetLogFile(), "\n");} 
    #define CCB_PWARNING(...) { printf("[    WARNING    ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                            printf(__VA_ARGS__); printf("\n"); }
#else
    #define CCB_FWARNING(...)
    #define CCB_PWARNING(...)
#endif


// INFOS & NOTIMP
#if CCB_LOGLEVEL >= 2
    #define CCB_FINFO(...) { fprintf(ccb_GetLogFile(), "[     INFO      ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                         fprintf(ccb_GetLogFile(), __VA_ARGS__); fprintf(ccb_GetLogFile(), "\n");}
    #define CCB_PINFO(...) { printf("[     INFO      ] %s::%s()::l%d: ", __FILE__, __FUNCTION__, __LINE__); \
                         printf(__VA_ARGS__); printf("\n"); }

    #define CCB_PNOT_IMPLEMENTED() {fprintf(GetLogFile(), "[NOT IMPLEMENTED] %s::%s()::l%d\n", __FILE__, __FUNCTION__, __LINE__);}
    #define CCB_FNOT_IMPLEMENTED() {printf("[NOT IMPLEMENTED] %s::%s()::l%d\n", __FILE__, __FUNCTION__, __LINE__);}

#else
    #define CCB_FINFO(...)
    #define CCB_PINFO(...)
    #define CCB_PNOT_IMPLEMENTED()
    #define CCB_FNOT_IMPLEMENTED()
#endif


// NOT NULL
#define CCB_FNOTNULL(ptr, ...) if (ptr == NULL) CCB_ERROR(__VA_ARGS__)
#define CCB_PNOTNULL(ptr, ...) if (ptr == NULL) CCB_ERROR(__VA_ARGS__)
#define CCB_FCHECK(cond, ...) if (!(cond)) CCB_ERROR(__VA_ARGS__)
#define CCB_PCHECK(cond, ...) if (!(cond)) CCB_ERROR(__VA_ARGS__)

// DEFAULT VERSIONS
#if CCB_LOGTYPE == 1
    #define CCB_ERROR(...) {CCB_FERROR(__VA_ARGS__); exit(-1);}
    #define CCB_WARNING(...) CCB_FWARNING(__VA_ARGS__)
    #define CCB_INFO(...) CCB_FINFO(__VA_ARGS__)
    #define CCB_NOT_IMPLEMENTED() CCB_FNOT_IMPLEMENTED()
    #define CCB_NOTNULL(ptr, ...) CCB_FNOTNULL(ptr, __VA_ARGS__)
    #define CCB_CHECK(cond, ...) CCB_FCHECK(cond, __VA_ARGS__)

#else
#if CCB_LOGTYPE == 2
    #define CCB_ERROR(...) {CCB_PERROR(__VA_ARGS__) CCB_FERROR(__VA_ARGS__) exit(-1); }
    #define CCB_WARNING(...) CCB_PWARNING(__VA_ARGS__) CCB_FWARNING(__VA_ARGS__)
    #define CCB_INFO(...) CCB_PINFO(__VA_ARGS__) CCB_FINFO(__VA_ARGS__)
    #define CCB_NOT_IMPLEMENTED() CCB_PNOT_IMPLEMENTED() CCB_FNOT_IMPLEMENTED()
    #define CCB_NOTNULL(ptr, ...) CCB_PNOTNULL(ptr, __VA_ARGS__) CCB_FNOTNULL(ptr, __VA_ARGS__)
    #define CCB_CHECK(cond, ...) CCB_PCHECK(cond, __VA_ARGS__) CCB_FCHECK(cond, __VA_ARGS__)

#else
    #define CCB_ERROR(...) {CCB_PERROR(__VA_ARGS__) exit(-1);}
    #define CCB_WARNING(...) CCB_PWARNING(__VA_ARGS__)
    #define CCB_INFO(...) CCB_PINFO(__VA_ARGS__)
    #define CCB_NOT_IMPLEMENTED() CCB_PNOT_IMPLEMENTED()
    #define CCB_NOTNULL(ptr, ...) CCB_PNOTNULL(ptr, __VA_ARGS__)
    #define CCB_CHECK(cond, ...) CCB_PCHECK(cond, __VA_ARGS__)

#endif
#endif