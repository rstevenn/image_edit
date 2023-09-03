#ifndef __CONFIG_IMAGE_EDIT_H__
#define __CONFIG_IMAGE_EDIT_H__

#define INPUT_FILE "./inp/h849lhurffr71.jpg"
#define OUTPUT_FILE "./out/out.png"

#ifdef LOGLEVEL
#undef LOGLEVEL
#endif


#ifdef LOGTYPE
#undef LOGTYPE
#endif

#ifdef DEBUGIMEDIT

#define LOGLEVEL 2
#define LOGTYPE 2

#else

#define LOGLEVEL 1
#define LOGTYPE 1

#endif


#endif // __CONFIG_IMAGE_EDIT_H__
