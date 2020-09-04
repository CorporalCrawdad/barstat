#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>

#include "status.h"
#include "config.h"
#include "util.h"

const char *getbatlvl(void);
const char *getnetu(void);
const char *getnetd(void);
const char *gettime(void);
const char *getsong(void);
const char *getvolm(void);

Display *dpy;
Window   root;

void
cleanup()
{
	XStoreName(dpy, root, '\0');
	XCloseDisplay(dpy);
	return;
}

void
clucatch(int signum)
{
	exit(0);
}

int
main()
{
	size_t   len = 0, buflen;
	char     status[1024] = {'\0'};

	if ( (dpy=XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "ERROR: could not open display\n");
		exit(1);
	}
	root = XRootWindow(dpy,DefaultScreen(dpy));
	atexit(cleanup);
	signal(SIGINT, clucatch);
	signal(SIGKILL, clucatch);

	for (;;) {
		status[0] = '\0';
		len = 0;
		for (int i=0; i<LEN(items); i++) {
			len += snprintf(status + len, sizeof(status) - len,
					"%s", items[i].prefix);
			if (len >= sizeof(status)) 
				status[sizeof(status) - 1] = '\0';
			
			buflen = len;
			len += snprintf(status + len, 
					(((sizeof(status) - len) < items[i].width) || !items[i].width)
						?sizeof(status) - len
						: items[i].width + 1,
					"%*s",  items[i].width, items[i].getfunc());

			len = strlen(status);
			if (len >= sizeof(status)) 
				status[sizeof(status) - 1] = '\0';

			len += snprintf(status + len, sizeof(status) - len,
					"%s", items[i].suffix);
			if (len >= sizeof(status)) 
				status[sizeof(status) - 1] = '\0';
		}
//		printf("\e[2K\e[s %s\e[u", status);
//		printf("%s\n\n", status);
//		fflush(stdout);
		XStoreName(dpy, root, status);
		XFlush(dpy);
		usleep(100000);
	}
	return 0;
}
