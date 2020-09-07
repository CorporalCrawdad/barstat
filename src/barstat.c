#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
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
char     pipeb[512] = {'\0'};
int      done=0;

void
cleanup()
{
	XStoreName(dpy, root, '\0');
	XCloseDisplay(dpy);
	return;
}

void
sighandle(int signum)
{
	done=1;
}

int
main()
{
	size_t   len = 0, buflen;
	char     status[1024] = {'\0'};
	char     pbuf[1024] = {'\0'};
	char     prb[1024] = {'\0'};
	int      pipefd = 0;
	int      readlen = 0;
	struct   sigaction sa;

	atexit(cleanup);
	sa.sa_handler = sighandle;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa, NULL)==-1) {
		fprintf(stderr, "ERROR: could not set sighandler\n");
		exit(1);
	}

	if ( (dpy=XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "ERROR: could not open display\n");
		exit(1);
	}
	root = XRootWindow(dpy,DefaultScreen(dpy));

	while (!done) {
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
	cleanup();
	remove(FIFONAME);
	exit(0);
	return 0;
}
