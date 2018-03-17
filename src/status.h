#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

// FUNCTIONS
// query interfaces
const char *getbatlvl(void);
const char *getnetu(void);
const char *getnetd(void);
const char *gettime(void);
const char *getsong(void);
const char *getvolm(void);
const char *getramp(void);
const char *getramu(void);
const char *getramt(void);

//typedefs
typedef struct {
        const char* (*getfunc)(void);
        const char* prefix;
        int width;
        const char* suffix;
} baritem;
