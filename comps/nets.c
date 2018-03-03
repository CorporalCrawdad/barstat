#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"

#define BUF_SZ 1024
#define HIST_CNT 5

static void  net_init(void);
static void  net_updt(const char *net_pref);
       void  net_dest(void);
const  char *getnets(void*);

static char  netsbuffer[ BUF_SZ ];
static int  *ulhist[ HIST_CNT ];
static int  *dlhist[ HIST_CNT ];
static int   histit = -1;
static char *savprefix = NULL;
       char  buf[1024];

static void
net_init(void)
{
	for (int i=0; i<sizeof(netsbuffer); i++)
		netsbuffer[i] = 0;
	for (int i=0; i<HIST_CNT; i++) {
		ulhist[i] = calloc (1, sizeof(int));
		dlhist[i] = calloc (1, sizeof(int));
	}
	histit = 0;
	atexit (net_dest);
}

void
net_dest(void)
{
	for (int i=0; i<HIST_CNT; i++) {
		free (ulhist[i]);
		free (dlhist[i]);
	}
}

// modes: 0 for dl, 1 for ul
const char *
getnets(void* in)
{
	int upload = *(int*)in;
	int avg = 0;
	int **histp = NULL;

	if (histit < 0) {
		net_init ();
	}

	// update both usage arrays once per pair of calls
	if (upload)
		net_updt (NULL, upload);

	// array is not full, can't process avg yet
	if (histit+1 < HIST_CNT) {
		return bprintf ("0");
	}

	if (upload)
		histp = ulhist;
	else
		histp = dlhist;

	// get total differences between each data point
	for (int i=1; i<HIST_CNT; i++) {
		avg += *histp[i] - *histp[i-1];
	}
	// average them out and return the result
	return bprintf ("%i", (avg / (HIST_CNT-1)));
}

static void
net_updt(const char *net_pref)
{
	FILE* procnetdev = fopen("/proc/net/dev", "r");
	int   ret = 0;
	char  cbuf = '\0';
	int   netul, netdl;
	char  netsbuffer[BUF_SZ];
	char *token;

	if (!net_pref) {
		if (!savprefix) {
			fprintf( stderr, "net_updt: attempting to update nets without prefix\n");
			return;
		}
		else
		       net_pref = savprefix;
	}
	else
		savprefix =  net_pref;
	

	if (procnetdev == NULL)
		return ;
	
	// scroll through /proc/net/dev to interface
	while (strncmp(netsbuffer, net_pref, 3))
		fgets (netsbuffer, BUF_SZ, procnetdev);

	token = strtok (netsbuffer, " ");
	while (token != NULL) {
		printf ("%s\n", token);
		token = strtok (NULL, " ");
	}
}

int
main(void)
{
	net_updt("wlp");
	return 0;
}
