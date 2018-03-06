#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *getbatlvl(void);
const char *getnetu(void);
const char *getnetd(void);
const char *gettime(void);
const char *getsong(void);

#include "util.h"

int
main()
{
	size_t len = 0;
	char   status[1024] = {'\0'};

	for (;;) {
		status[0] = '\0';
		len = 0;
		len += snprintf(status + len, sizeof(status) - len,
		                "%s%%🔋\t", getbatlvl());
		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "%sU|", getnetu());
		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "%sD🖧\t", getnetd());
		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "%s♫\t", getsong());

		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		len += snprintf(status + len, sizeof(status) - len,
		                "⏲ [%s]", gettime());

		if (len >= sizeof(status)) {
			status[sizeof(status) - 1] = '\0';
		}
		printf("\e[2K\e[s %s\e[u", status);
		fflush(stdout);
		sleep(1);
	}
	return 0;
}
