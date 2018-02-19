#include "status.h"
#include "config.h"
#include "bat.h"

// (local) global vars
static int  dl, ul;
int verbosity;

static int net_updt(void);
static int remtrans(int, int);
static char netsbuffer[CHAR_BUF];
static int * ulhist[HIST_CNT];
static int * dlhist[HIST_CNT];
static int  histit;

static struct mpd_connection *mpconn = NULL;
static struct mpd_status     *mpstatus;
static struct mpd_song       *mpsong;
static enum   mpd_state      mpstate = 0;
static char   *mpsong_name;

#ifdef GET_ARTIST
static char   *mpartist_name;
#endif

// (local) functions
int  audio_volume(audio_volume_action, long*);
int  main(int argc, char* argv[]);
void stop(int);
void prepend(char*, const char, int);



void prepend(char* s, const char t, int len)
{
	size_t i;

	memmove (s + len, s, strlen(s)+1);

	for (i = 0; i < len; ++i)
	{
		s[i] = t;
	}
}

int gettime(char* out) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char month[4];

	switch (tm.tm_mon)
	{
		case 0:
			strcpy(month, "Jan");
			break;
		case 1:
			strcpy(month, "Feb");
			break;
		case 2:
			strcpy(month, "Mar");
			break;
		case 3:
			strcpy(month, "Apr");
			break;
		case 4:
			strcpy(month, "May");
			break;
		case 5:
			strcpy(month, "Jun");
			break;
		case 6:
			strcpy(month, "Jul");
			break;
		case 7:
			strcpy(month, "Aug");
			break;
		case 8:
			strcpy(month, "Sep");
			break;
		case 9:
			strcpy(month, "Oct");
			break;
		case 10:
			strcpy(month, "Nov");
			break;
		case 11:
			strcpy(month, "Dec");
			break;
	}


	snprintf(out, STAT_LEN, "%s %d %d %.2d:%.2d:%.2d", month, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);

	return 0;
}

void stop(int retval) {
	mop_dest();
	net_dest();
	exit(retval);
}

int getnetu(char* outs) {
	// convert dl speed to highest metric possible
	if (ul)
	{
		if(ul/1000)
		{
			if(ul/1000000)
			{
				if (ul/1000000000)
				{
					snprintf(outs, CHAR_BUF, "%.1fGB", ((float)ul/1000000000.00));
				}
				else
					snprintf(outs, CHAR_BUF, "%.1fMB", ((float)ul/1000000.00));
			}
			else
				snprintf(outs, CHAR_BUF, "%.1fKB", ((float)ul/1000.00));
		}
		else
			snprintf(outs, CHAR_BUF, "%dB", ul);
	}
	else
		snprintf(outs, CHAR_BUF, "%dB", ul);
	return 0;
}

int getnetd(char* outs) {
	// convert dl speed to highest metric possible
	if (dl)
	{
		if(dl/1000)
		{
			if(dl/1000000)
			{
				if (dl/1000000000)
				{
					snprintf(outs, CHAR_BUF, "%.1fGB", ((float)dl/1000000000.00));
				}
				else
					snprintf(outs, CHAR_BUF, "%.1fMB", ((float)dl/1000000.00));
			}
			else
				snprintf(outs, CHAR_BUF, "%.1fKB", ((float)dl/1000.00));
		}
		else
			snprintf(outs, CHAR_BUF, "%dB", dl);
	}
	else
		snprintf(outs, CHAR_BUF, "%dB", dl);
	return 0;
}


/*
  Drawbacks. Sets volume on both channels but gets volume on one. Can be easily adapted.
 */
int audio_volume(audio_volume_action action, long* outvol)
{
    snd_mixer_t* handle;
    snd_mixer_elem_t* elem;
    snd_mixer_selem_id_t* sid;

    static const char* mix_name = "Master";
    static const char* card = "default";
    static int mix_index = 0;

    long pmin, pmax;
    long get_vol, set_vol;
    float f_multi;
    int ret;

    snd_mixer_selem_id_alloca(&sid);

    //sets simple-mixer index and name
    snd_mixer_selem_id_set_index(sid, mix_index);
    snd_mixer_selem_id_set_name(sid, mix_name);

        if ((snd_mixer_open(&handle, 0)) < 0)
        return -1;
    if ((snd_mixer_attach(handle, card)) < 0) {
        snd_mixer_close(handle);
        return -2;
    }
    if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
        snd_mixer_close(handle);
        return -3;
    }
    ret = snd_mixer_load(handle);
    if (ret < 0) {
        snd_mixer_close(handle);
        return -4;
    }
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_close(handle);
        return -5;
    }

    long minv, maxv;

    snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
//    fprintf(stderr, "Volume range <%i,%i>\n", minv, maxv);

    if(action == AUDIO_VOLUME_GET) {
        if(snd_mixer_selem_get_playback_volume(elem, 0, outvol) < 0) {
            snd_mixer_close(handle);
            return -6;
        }

//        fprintf(stderr, "Get volume %i with status %i\n", *outvol, ret);
        /* make the value bound to 100 */
        *outvol -= minv;
        maxv -= minv;
        minv = 0;
        *outvol = 100 * (*outvol) / maxv; // make the value bound from 0 to 100
    }

    if (*outvol == 100) *outvol = 99;
    snd_mixer_close(handle);
    return 0;
}

int getvol(char* outs)
{
	long lvol;
	audio_volume(AUDIO_VOLUME_GET, &lvol);
	snprintf (outs, STAT_LEN, "%d", lvol);
	return lvol;
}


int net_init(void) {
	for (int i=0; i<CHAR_BUF; i++)
		netsbuffer[i] = 0;
	//ulhist = malloc(HIST_CNT*sizeof(int*));
	//dlhist = malloc(HIST_CNT*sizeof(int*));
	for (int i=0; i<HIST_CNT; i++) {
		ulhist[i] = malloc(sizeof(int));
		dlhist[i] = malloc(sizeof(int));
		*(ulhist[i]) = 0;
		*(dlhist[i]) = 0;
	}
	histit = 0;
}

int net_dest(void) {
	for (int i=0; i<HIST_CNT; i++) {
		free(ulhist[i]);
		free(dlhist[i]);
	}
}

int getnets(int* netdl, int* netul) {
	net_updt();
	// array is not full, not enough data to process yet
	if (histit+1 < HIST_CNT) {
		*netdl = 0;
		*netul = 0;
		return 0;
	}

	// get difference between each data point
	// [0] is UPLOAD
	// [1] is DOWNLOAD
	int diffs[2][HIST_CNT-1];
	int netavg[2] = {0,0};
	for (int i=1; i<HIST_CNT; i++) {
		diffs[0][i-1] = *ulhist[i] - *ulhist[i-1];
		diffs[1][i-1] = *dlhist[i] - *dlhist[i-1];
	}
	// get average difference
	for (int i=0; i<HIST_CNT-1; i++) {
		netavg[0] += diffs[0][i];
		netavg[1] += diffs[1][i];
	}
	netavg[0] = netavg[0] / (HIST_CNT-1);
	netavg[1] = netavg[0] / (HIST_CNT-1);

	*netul = netavg[0];
	*netdl = netavg[1];
}

static int net_updt(void) {
	FILE* procnetdev = fopen("/proc/net/dev", "r");
	if (procnetdev == NULL) return -1;
	int ret = 0;
	char cbuf = ' ';
	int netul, netdl;

	// scroll through /proc/net/dev to interface
	// defined in config.h as net_pref
	while (strncmp(netsbuffer, net_pref, 3))
		for (int i=0; i<CHAR_BUF; i++) {
			netsbuffer[i] = fgetc (procnetdev);
			if (netsbuffer[i] == EOF)
				return 1;
			if (isspace(netsbuffer[i]))
				break;
		}

	// next element will be total dl'd bytes	
	for (int i=0; i<CHAR_BUF; i++) {
		netsbuffer[i] = fgetc (procnetdev);
		if (isspace(netsbuffer[i]))
			break;
	}
	netdl = atoi(netsbuffer);

	// eats the next 7 columns to get to ul'd bytes
	for (int i=0; i<7; i++) {
		while (cbuf == ' ') {
			cbuf = fgetc(procnetdev);
		}
		while ( cbuf != ' '  ) {
			cbuf = fgetc(procnetdev);
		}
		cbuf = ' ';
	}
	// eat final whitespace before grabbing ul bytes
	while (cbuf == ' ') {
		cbuf = fgetc(procnetdev);
	}

	// whitespace eater grabs first char of desired info
	// put it in [0] and read the rest from fgetc
	netsbuffer[0] = cbuf;
	ret = 0;
	if (cbuf != '0') // something has been uploaded
		while (ret == 0) {
			for (int i=1; i<CHAR_BUF; i++) {
				netsbuffer[i] = fgetc (procnetdev);
				if (isspace(netsbuffer[i]))
					break;
			}
			ret = atoi(netsbuffer);
		}
 	netul = ret;
	remtrans(netul, netdl);
}

static int remtrans(int ul, int dl) {
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
		int* tmp;
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
	// apparently something went wrong
	return 1;
}

int getsong(char* outs) {
	if (verbosity)
		puts("getting song");
	if(!mpconn) {
		fprintf(stderr, "Attempting to read MPD status without successful connection\n");
		strcpy(outs, "NOT PLAYING");
		return 1;
	}
	
	mpstatus = mpd_run_status(mpconn);
	mpsong   = mpd_run_current_song(mpconn);
	mpstate  = mpd_status_get_state(mpstatus);

	if (!mpsong) 
	{
		fprintf(stderr, "Could not get current song\n");
		strcpy(outs, "NOT PLAYING");
	}
	else if (!mpstatus)
	{
		fprintf(stderr, "Could not get MPD status\n");
		strcpy(outs, "NOT PLAYING");
	}
	else
	{
		// get song info
		mpsong_name = (char *) mpd_song_get_tag(mpsong, MPD_TAG_TITLE, 0);
#ifdef GET_ARTIST
		mpartist_name = (char *) mpd_song_get_tag(mpsong, MPD_TAG_ARTIST, 0);

		strncpy(outs, mpartist_name, STAT_LEN); 
		strncat(outs, mpsong_name, STAT_LEN);
#else
		strncpy(outs, mpsong_name, STAT_LEN);
#endif
	}
	return 0;
}

int mop_init(void) {
	if (verbosity)
		puts("hello");
	if (!mpconn) {
		mpconn = mpd_connection_new(NULL, 0, 0);
		if (mpconn)
		{
			if (mpd_connection_get_error(mpconn))
			{
				fprintf(stderr, "Connection failure! No local MPD session running!\n");
				mpconn = NULL;
			}
			if (verbosity)
				printf("Connected!\n");

		}	
		else
		{
			fprintf(stderr, "Connection failure! Out of memory!!\n");
			return 1;
		}
	}
	return (mpconn) ? 0 : 1;
}

int mop_dest(void) {
	if (mpconn)
	{
		puts("Disconnecting from MPD...\n");
		mpd_status_free(mpstatus);
		mpd_connection_free(mpconn);
		mpd_song_free(mpsong);
	}
	mpsong_name = NULL;
#ifdef GET_ARTIST
	mpartist_name = NULL;
#endif

	return 0;
}

int
getbat(char* outs)
{
	if (!(BT_FULL) || !(BT_NOW)) {
		outs = "";
		return -1;
	}
	FILE* bat_full, *bat_now;

	bat_full = fopen(BT_FULL, "r");
	bat_now = fopen(BT_NOW, "r");
	
	if (!(bat_full) || !(bat_now)) {
		outs = "";
		return -1;
	}
	char buf[25];

	int full = 0;
	int now  = 0;

	full = (fgets(buf, 25, bat_full)) ? atoi(buf) : 0;
	now  = (fgets(buf, 25, bat_now))  ? atoi(buf) : 0;

	snprintf(outs, STAT_LEN, "%i", ((now*100)/full));
	return 0;
}


int
main(int argc, char* argv[]) 
{
	char status[STAT_LEN]; /* change length (42) as needed */
	char buffer[STAT_LEN];
	char c;
	Display *dpy;
	Window root;

	if ( (dpy=XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "ERROR: could not open display\n");
		exit(1);
	}
	root = XRootWindow(dpy,DefaultScreen(dpy));

	switch (argc)
	{
		case 3:
			printf("%s: Usage\n%s [-v]", argv[0], argv[0]);
			break;
		case 2:
			verbosity = atoi(argv[1]);
			break;
		default:
			verbosity = 0;
			break;
	}

	// initializations
	signal(SIGINT, stop);
	net_init();
	mop_init();

	for(;;) {
		for (int i=0; i<STAT_LEN; i++) {
			status[i] = '\0';
			buffer[i] = '\0';
		}
		// check connection is still good or for new connection
		mop_init();

		// udpate nets arrays and fill up/dwn for polling in
		// user defined structure
		getnets(&dl, &ul);
	
		for (int i=0; i < LENGTH(items); i++) {
			// flush buffer
			for (int i=0; i<(STAT_LEN); i++)
				buffer[i] = '\0';
			// append prefix to status string
			strcat(status, items[i].prefix);
			// get system info via baritem.getfunc
			items[i].getfunc(buffer);
			// space out / truncate retrieved system info to fit given width
			if (items[i].width) {
				if ((strlen(buffer) + strlen(items[i].suffix)) < items[i].width)
					prepend(buffer, ' ', (items[i].width - strlen(items[i].suffix) - strlen(buffer)));
				strncat(status, buffer, (items[i].width - strlen(items[i].suffix)));
			} else {
				strncat(status, buffer, STAT_LEN);
			}
			// append suffix
			strcat(status, items[i].suffix);
		};
	
		// put results to stdout if debugging
		// set as root title otherwise
#ifdef DEBUG
		puts(status);
		printf("\n");
		if (verbosity)
			getc(stdin);
#else
		XStoreName(dpy,root,status);
		XFlush(dpy);
#endif
		sleep(1);
	}

	XCloseDisplay(dpy);
	stop(0);
	return 0;
}
