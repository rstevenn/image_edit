#ifndef __CONFIG_IMAGE_EDIT_H__
#define __CONFIG_IMAGE_EDIT_H__

#define INPUT_FILE "./inp/h849lhurffr71.jpg"
#define OUTPUT_FILE "./out/out.png"

#ifdef CCB_LOGLEVEL
#undef CCB_LOGLEVEL
#endif


#ifdef CCB_LOGTYPE
#undef CCB_LOGTYPE
#endif

#ifdef DEBUGIMEDIT

#define CCB_LOGLEVEL 2
#define CCB_LOGTYPE 2

#else

#define CCB_LOGLEVEL 1
#define CCB_LOGTYPE 1

#endif


#endif // __CONFIG_IMAGE_EDIT_H__
