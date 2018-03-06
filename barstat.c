#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

const char *getbatlvl(void);
const char *getnetu(void);
const char *getnetd(void);
const char *gettime(void);
const char *getsong(void);
const char *getvolm(void);

Display *dpy;
Window   root;

#include "util.h"

void
cleanup()
{
	XCloseDisplay(dpy);
	return;
}

int
main()
{
	size_t   len = 0;
	char     status[1024] = {'\0'};

	if ( (dpy=XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "ERROR: could not open display\n");
		exit(1);
	}
	root = XRootWindow(dpy,DefaultScreen(dpy));
	atexit(cleanup);

	for (;;) {
		status[0] = '\0';
		len = 0;
		/*len += snprintf(status + len, sizeof(status) - len,
		                "🔋%s%%    ", getbatlvl());
		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}*/
		len += snprintf(status + len, sizeof(status) - len,
		                "🖧%s↾", getnetu());
		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                " %s⇂    ", getnetd());
		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "♫%s    ", getsong());

		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "🔊%s    ", getvolm());

		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "⏲[%s]", gettime());

		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
//		printf("\e[2K\e[s %s\e[u", status);
//		fflush(stdout);
		XStoreName(dpy, root, status);
		XFlush(dpy);
		sleep(1);
	}
	return 0;
}
