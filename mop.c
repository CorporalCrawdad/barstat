#include <mpd/client.h>
#include <mpd/stats.h>
#include "status.h"

static struct mpd_connection *conn = NULL;
static struct mpd_status     *status;
static struct mpd_song       *song;
static enum   mpd_state      state = 0;
static char   *song_name;

#ifdef GET_ARTIST
static char   *artist_name;
#endif

int getsong(char* outs) {
	puts("getting song");
	if(!conn) {
		fprintf(stderr, "Attempting to read MPD status without successful connection\n");
		return 1;
	}
	
	status = mpd_run_status(conn);
	song   = mpd_run_current_song(conn);
	state  = mpd_status_get_state(status);

	if (!song) 
	{
		fprintf(stderr, "Could not get current song\n");
		strcpy(outs, "NOT PLAYING");
	}
	else if (!status)
	{
		fprintf(stderr, "Could not get MPD status\n");
		strcpy(outs, "NOT PLAYING");
	}
	else
	{
		// get song info
		song_name = (char *) mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
#ifdef GET_ARTIST
		artist_name = (char *) mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);

		strncpy(outs, artist_name, CHAR_BUF);
		strncat(outs, song_name, CHAR_BUF);
#else
		strncpy(outs, song_name, CHAR_BUF);
#endif
	}
	return 0;
}

int mop_init(void) {
	if (!conn) {
		conn = mpd_connection_new(NULL, 0, 0);
		if (conn)
			printf("Connected!\n");
		else
			fprintf(stderr, "Connection failure! No local MPD session running!\n");
	}
	return (conn) ? 0 : 1;
}

int mop_dest(void) {
	puts("Disconnecting from MPD...\n");
	mpd_status_free(status);
	mpd_connection_free(conn);
	mpd_song_free(song);

	song_name = NULL;
#ifdef GET_ARTIST
	artist_name = NULL;
#endif

	return 0;
}
