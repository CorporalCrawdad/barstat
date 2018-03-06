#include <stdio.h>
#include <stdlib.h>
#include <mpd/client.h>
#include <mpd/stats.h>

#include "../util.h"

static int   mop_init(void);
static void  mop_dest(void);
const  char *getsong(void);

static struct mpd_connection *mpconn   = NULL;
static struct mpd_status     *mpstatus;
static struct mpd_song       *mpsong;
static enum   mpd_state       mpstate  = 100;
static        char           *mpsong_name;

static int
mop_init()
{
	if (mpstate == 100) {
		atexit(mop_dest);
		mpstate = 0;
	}
	if (!mpconn) {
		mpconn = mpd_connection_new(NULL, 0, 1000);
		if (mpconn) {
			if (mpd_connection_get_error(mpconn)) {
				fprintf(stderr, "Connection failure! Error code %i.\n",
					       	mpd_connection_get_error(mpconn));
				mpconn = NULL;
			}
			// connected!
		} else {
			fprintf(stderr, "Connection failure! Out of memory!!\n");
			return 1;
		}
	}
	return (mpconn) ? 0 : 1;
}

static void
mop_dest()
{
	if (mpconn) {
		puts("Disconnecting from MPD...\n");
		mpd_status_free(mpstatus);
		mpd_connection_free(mpconn);
		mpd_song_free(mpsong);
	}
	mpsong_name = NULL;
	return;
}

const char *
getsong()
{
	mop_init();
	if (!mpconn)
		return bprintf("NOT RUNNING");
	
	mpstatus = mpd_run_status(mpconn);
	mpsong   = mpd_run_current_song(mpconn);
	mpstate  = mpd_status_get_state(mpstatus);

	if (!mpsong) {
		return bprintf("NOT PLAYING");
	} else if (!mpstatus) {
		return bprintf("NOT PLAYING");
	} else {
		switch (mpstate) {
			case MPD_STATE_STOP:
				return bprintf("%s⏹", (char *) mpd_song_get_tag(mpsong, MPD_TAG_TITLE, 0));
			case MPD_STATE_PLAY:
				return bprintf("%s►", (char *) mpd_song_get_tag(mpsong, MPD_TAG_TITLE, 0));
			case MPD_STATE_PAUSE:
				return bprintf("%s⏸", (char *) mpd_song_get_tag(mpsong, MPD_TAG_TITLE, 0)); 
			case MPD_STATE_UNKNOWN:
			default:
				return bprintf("UNKNOWN");
		}
	}
}
