#ifndef STATUS_H
#define STATUS_H

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include <mpd/client.h>
#include <mpd/stats.h>

#define CHAR_BUF 20
#define HIST_CNT 5
#define STAT_LEN 160 

// FUNCTIONS
// initializers
int mop_init(void);

// old query interfaces
int getchat(char*);
int getnets(int*,int*);
int getmail(char*);
int getmem(char*);
int getsong(char*);
int getvol(char*);
// query interfaces
const char *getnetu(void);
const char *getnetd(void);
const char *gettime(void);
const char *getbatlvl(void);

// destructors
int mop_dest(void);

//typedefs
typedef struct {
        int (*getfunc)(char*);
        const char* prefix;
        int width;
        const char* suffix;
} baritem;

typedef enum {
    AUDIO_VOLUME_SET,
    AUDIO_VOLUME_GET,
} audio_volume_action;
#endif
