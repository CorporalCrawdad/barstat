#include <stdio.h>
#include <time.h>

#include "../util.h"

const char*
gettime(void)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char *month;

	switch (tm.tm_mon)
	{
		case 0:
			month = "Jan";
			break;
		case 1:
			month = "Feb";
			break;
		case 2:
			month = "Mar";
			break;
		case 3:
			month = "Apr";
			break;
		case 4:
			month = "May";
			break;
		case 5:
			month = "Jun";
			break;
		case 6:
			month = "Jul";
			break;
		case 7:
			month = "Aug";
			break;
		case 8:
			month = "Sep";
			break;
		case 9:
			month = "Oct";
			break;
		case 10:
			month = "Nov";
			break;
		case 11:
			month = "Dec";
			break;
	}


	return bprintf("%s %d %d %.2d:%.2d:%.2d",
	               month, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour,
	               tm.tm_min, tm.tm_sec);
}

