#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util.h"

#define BUF_SZ 1024
#define HIST_CNT 3

static void  net_init(void);
static void  net_updt();
static int   remtrans(int ul, int dl);
       void  net_dest(void);
const  char *getnetu(void);
const  char *getnetd(void);
const  static int getnets(int);
static const char* procmedaddy(int);

static unsigned long  *ulhist[ HIST_CNT ];
static unsigned long  *dlhist[ HIST_CNT ];
static int   histit = -1;
static char  netsbuffer[ BUF_SZ ];
extern
const  char*  net_pref;
       char  buf[1024];

static void
net_init(void)
{
	for (int i=0; i<sizeof(netsbuffer); i++)
		netsbuffer[i] = 0;
	for (int i=0; i<HIST_CNT; i++) {
		ulhist[i] = calloc (1, sizeof(unsigned long));
		dlhist[i] = calloc (1, sizeof(unsigned long));
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
static int
getnets(int upload)
{
	int avg = 0;
	unsigned long **histp = NULL;

	if (histit < 0) {
		net_init ();
	}

	// update both usage arrays once per pair of calls
	if (upload)
		net_updt ();

	// array is not full, can't process avg yet
	if (histit+1 < HIST_CNT) {
		return 0;
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
	return (avg / (HIST_CNT-1));
}

static void
net_updt()
{
	FILE* procnetdev = fopen("/proc/net/dev", "r");
	int   ret = 0;
	char  cbuf = '\0';
	unsigned long netul, netdl;
	char  netsbuffer[BUF_SZ] = {'\0'};
	char *token;

	if (!net_pref) {
		fprintf( stderr, "net_updt: attempting to update nets without prefix\n");
		return;
	}

	if (procnetdev == NULL)
		return ;

	
	// scroll through /proc/net/dev to interface
	while (strncmp(netsbuffer, net_pref, 3)) {
		fgets (netsbuffer, BUF_SZ, procnetdev);
	}


	fclose (procnetdev);
	
	// the 1st and 9th fields are the only ones we're interested in...
	//printf("\n%s\n", netsbuffer);
	token = strtok (netsbuffer, " ");
	for (int i=0; token != NULL; i++) {
		switch (i) {
			case 1:
				netdl = atol (token);
				break;
			case 9:
				netul = atol (token);
				break;
		}
		token = strtok (NULL, " ");
	}
	remtrans(netul, netdl);
}

static int
remtrans(int ul, int dl)
{
	// less than HIST_CNT datapoints, fill array as normal
	if (histit+1 < HIST_CNT) {
		*ulhist[histit] = ul;
		*dlhist[histit] = dl;
		histit++;
		return 0;
	}
	// our array is full, so rotate all the pointers back one, fill the
	// oldest pointer with the newest value, and move that pointer to be
	// the newest value in array.
	else
	{
		long* tmp;
		tmp = ulhist[0];
		for (int i=0; (i+1)<HIST_CNT; i++) 
			ulhist[i] = ulhist[i+1];
		*tmp = ul;
		ulhist[HIST_CNT-1] = tmp;
		
		tmp = dlhist[0];
		for (int i=0; (i+1)<HIST_CNT; i++)
			dlhist[i] = dlhist[i+1];
		*tmp = dl;
		dlhist[HIST_CNT-1] = tmp;
		return 0;
	}
}

static
const char*
procmedaddy(int speed)
{
	char buf[6] = {'\0'};
	
	if (speed)
	{
		if(speed/1000)
		{
			if(speed/1000000)
			{
				if (speed/1000000000)
				{
					//return bprintf ("%.1fGB", ((float)speed/1000000000.00));
					snprintf(buf, 5, "%.1f", ((float)speed/1000000000.00));
					if ( buf[3] == '.' )
						buf[3] = '\0';
					return bprintf ("%sGB", buf);
				} else { 
					//return bprintf ("%.1fMB", ((float)speed/1000000.00));
					snprintf(buf, 5, "%.1f", ((float)speed/1000000.00));
					if ( buf[3] == '.' )
						buf[3] = '\0';
					return bprintf ("%sMB", buf);
				}
			} else {
				//return bprintf ("%.1fKB", ((float)speed/1000.00));
				snprintf(buf, 5, "%.1f", ((float)speed/1000.00));
				if ( buf[3] == '.' ) 
					buf[3] = '\0';
				return bprintf ("%sKB", buf);
			}
		}
		else
			return bprintf ("%3dB", speed);
	}
	else
		return bprintf ("%3dB", speed);
	return bprintf("%i", 0);
}


const char *
getnetu(void)
{
	int speed = getnets(1);
	return procmedaddy(speed);
}

const char *
getnetd(void)
{
	int speed = getnets(0);
	return procmedaddy(speed);
}
