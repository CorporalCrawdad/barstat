#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util.h"

#define BUF_SZ 1024
#define HIST_CNT 3
#define MAX_IFACES 6

struct ifacehist
{
	unsigned long  *ulhist[ HIST_CNT ];
	unsigned long  *dlhist[ HIST_CNT ];
	char  netsbuffer[ BUF_SZ ];
	int   histit;
	char  net_pref[7];
}ifaces[MAX_IFACES];

static void  net_init(void);
static void  net_updt(struct ifacehist);
static int   remtrans(int ul, int dl, struct ifacehist);
       void  net_dest(void);
const  char *getnetu(void);
const  char *getnetd(void);
const  static int getnets(int);
static const char* procmedaddy(int);

int tracked_ifaces = -1;

extern
       char  buf[1024];

static void
net_init(void)
{
	for (int ii=0; ii<MAX_IFACES; ii++) {
		for (int i=0; i<sizeof(ifaces[ii].netsbuffer); i++)
			ifaces[ii].netsbuffer[i] = 0;
		for (int i=0; i<HIST_CNT; i++) {
			ifaces[ii].ulhist[i] = calloc (1, sizeof(unsigned long));
			ifaces[ii].dlhist[i] = calloc (1, sizeof(unsigned long));
		}
		ifaces[ii].histit = 0;
	}
	atexit (net_dest);

	// probe file and setup ifacehist prefixes
	FILE* procnetdev = fopen("/proc/net/dev", "r");
	char *prefbuf = (char*) malloc(256*sizeof(char));

	fgets(prefbuf, 256, procnetdev);
	fgets(prefbuf, 256, procnetdev);
	while (!feof(procnetdev)) {
		tracked_ifaces++;
		fgets(prefbuf, 256, procnetdev);
		snprintf(ifaces[tracked_ifaces].net_pref, 7, prefbuf);
	}
	fclose(procnetdev);
	free(prefbuf);

}

void
net_dest(void)
{
	for (int ii=0; ii<MAX_IFACES; ii++)
		for (int i=0; i<HIST_CNT; i++) {
			free (ifaces[ii].ulhist[i]);
			free (ifaces[ii].dlhist[i]);
		}
}

// modes: 0 for dl, 1 for ul
static int
getnets(int upload)
{
	int avg = 0;
	unsigned long **histp = NULL;

	if (tracked_ifaces < 0) {
		net_init ();
	}

	// update both usage arrays of all interfaces once per pair of calls
	if (upload)
		for (int i=-1; i<tracked_ifaces; i++)
			net_updt (ifaces[tracked_ifaces]);

	// array is not full, can't process avg yet
	for (int i=0; i<tracked_ifaces; i++)
		if (ifaces[i].histit+1 < HIST_CNT) {
			return 0;
		}

	for (int i=0; i<tracked_ifaces; i++) {
		if (upload)
			histp = ifaces[i].ulhist;
		else
			histp = ifaces[i].dlhist;
	
		// get total differences between each data point
		for (int ii=1; ii<HIST_CNT; ii++) {
			avg += *histp[ii] - *histp[ii-1];
		}
	}
	// average them out and return the result
	return (avg / ((HIST_CNT-1)*tracked_ifaces));
}

static void
net_updt(struct ifacehist this)
{
	FILE* procnetdev = fopen("/proc/net/dev", "r");
	int   ret = 0;
	char  cbuf = '\0';
	unsigned long netul, netdl;
	char  netsbuffer[BUF_SZ] = {'\0'};
	char *token;

	if (!this.net_pref) {
		fprintf( stderr, "net_updt: attempting to update nets without prefix\n");
		return;
	}

	if (procnetdev == NULL) {
		fprintf( stderr, "net_updt: could not open /proc/net/dev");
		return ;
	}

	
	// scroll through /proc/net/dev to interface
	while (strncmp(netsbuffer, this.net_pref, 6) && !feof(procnetdev)) {
		fgets (netsbuffer, BUF_SZ, procnetdev);
	}
	if (feof(procnetdev)) {
		fprintf( stderr, "net_updt: hit eof without match");
		fclose(procnetdev);
		return;
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
	printf("%s: %i, %i\n", this.net_pref, netul, netdl);
	remtrans(netul, netdl, this);
}

static int
remtrans(int ul, int dl, struct ifacehist this)
{
	// less than HIST_CNT datapoints, fill array as normal
	if (this.histit+1 < HIST_CNT) {
		*this.ulhist[this.histit] = ul;
		*this.dlhist[this.histit] = dl;
		this.histit++;
		return 0;
	}
	// our array is full, so rotate all the pointers back one, fill the
	// oldest pointer with the newest value, and move that pointer to be
	// the newest value in array.
	else
	{
		long* tmp;
		tmp = this.ulhist[0];
		for (int i=0; (i+1)<HIST_CNT; i++) 
			this.ulhist[i] = this.ulhist[i+1];
		*tmp = ul;
		this.ulhist[HIST_CNT-1] = tmp;
		
		tmp = this.dlhist[0];
		for (int i=0; (i+1)<HIST_CNT; i++)
			this.dlhist[i] = this.dlhist[i+1];
		*tmp = dl;
		this.dlhist[HIST_CNT-1] = tmp;
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
