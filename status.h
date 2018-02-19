#ifndef STATUS_H
#define STATUS_H

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <string.h>
#include <time.h>
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

// MACROS
#define testcpy() if (strlen(status) > (STAT_LEN/2)) { \
			printf("\nstr overflow\n"); \
			return 1; \
		  } \
		  printf("\nstrlen(status):%d STAT_LEN:%d\n", strlen(status), STAT_LEN);
//strncat(status, buffer, STAT_LEN); 
#define LENGTH(X)	(sizeof X / sizeof X[0])

// FUNCTIONS
// initializers
int net_init(void);
int mop_init(void);

// query interfaces
int getchat(char*);
int getnetu(char*);
int getnetd(char*);
int getnets(int*,int*);
int getmail(char*);
int getmem(char*);
int getsong(char*);
int gettime(char*);
int getvol(char*);
int getbat(char*);

// destructors
int net_dest(void);
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
