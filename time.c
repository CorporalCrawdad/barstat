#include <stdio.h>
#include <time.h>

char*
gettime(void)
{
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

