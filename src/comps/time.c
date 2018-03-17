#include <stdio.h>
#include <time.h>

#include "../util.h"

const char *months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

const char*
gettime(void)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	const char *month;

	month = months[tm.tm_mon];

	return bprintf("%s %d %d %.2d:%.2d:%.2d",
	               month, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour,
	               tm.tm_min, tm.tm_sec);
}

