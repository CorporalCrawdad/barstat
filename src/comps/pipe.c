#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../util.h"

extern const char *fifoname;

char obuf[1024];
char rbuf[1024];
int  pipefd = -1;

void
initpipe()
{
	if (mkfifo(fifoname, 0666)) {
		if (errno != EEXIST) {
			fprintf(stderr, "ERROR: pipe cannot be created...\n");
			return;
		}
	}
	pipefd = open(fifoname, O_RDONLY|O_NONBLOCK);
	for (int i=0; i<1024; i++) {
		obuf[i] = rbuf[i] = '\0';
	}
}

void
updpipe(void)
{
	int len, olen; 
	if (pipefd <=0)
		initpipe();
	if (pipefd > 0) {
		len = read(pipefd, rbuf, 1024);
		if (len == -1)
			if (errno == EBADF) {
				close(pipefd);
				pipefd = -1;
			}
		if (len) {
			for (int i=0; i<1024; i++) {
				if (rbuf[i] == '\n')
					obuf[i] = ' ';
				if (rbuf[i] == '\0')
					break;
			}
			strncpy(obuf, rbuf, sizeof(obuf));
			memset(rbuf, '\0', sizeof(rbuf));
		}
	}
}

const char*
getpipe(void)
{
	updpipe();
	return bprintf("%s", obuf);
}
