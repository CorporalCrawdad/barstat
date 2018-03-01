#include <stdio.h>

#include "../util.h"

#define BUF_SZ 1024
#define HIST_CNT 5

static void net_init(void);
void net_dest(void);
const char* getnets(void);

static char netsbuffer[ BUF_SZ ];
static int *ulhist[ HIST_CNT ];
static int *dlhist[ HIST_CNT ];
static int histit = -1;

static void
net_init()
{
	for (int i=0; i<sizeof(netsbuffer); i++)
		netsbuffer[i] = 0;
	for (int i=0; i<HIST_CNT; i++) {
		ulhist[i] = calloc(sizeof(int));
		dlhist[i] = calloc(sizeof(int));
	}
	histit = 0;
	atexit(net_dest);
}


