#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "../util.h"

#define CNDSTR(x) (flags & x) ? "%s" : ""

void alrm_dest(void);

extern const char *alarm_time;

struct tm alarm_tm;
time_t *end = (void*)0;
time_t cur;

enum tflags {
	T_SEC =	1<<0,
	T_MIN = 1<<1,
	T_HRS = 1<<2,
	T_DAY = 1<<3,
};

struct tm
parse_alarm(const char *in)
{
	const char *fields[6];
	char instr[1024] = {'\0'};
	strncpy(instr, in, 1024);
	for (int i=0; i<6; i++) 
		fields[i] = strtok( (i ? nullptr : instr), " \t\n,.");
	if ( !fields[5] || (strtok(nullptr, " \t\n,.")))
		goto err;

	struct tm ret;

	ret.tm_sec = atoi(fields[0]);
	ret.tm_min = atoi(fields[1]);
	ret.tm_hour = atoi(fields[2]);
	ret.tm_mday = atoi(fields[3]);
	ret.tm_mon  = atoi(fields[4])-1;
	ret.tm_year = atoi(fields[5])-1900;
	
	return ret;
err:
	fprintf(stderr, "ALRM: incorrectly formatted alarm string\n");
	return ret;
}

const char*
getalrm(void)
{
	char out[256] = {'\0'};
	time(&cur);
	if (!end) {
		end = malloc(sizeof(time_t));
		atexit(alrm_dest);
		/*
		struct tm alarm_tm;
		alarm_tm.tm_sec = alarm_time[0];
		alarm_tm.tm_min = alarm_time[1];
		alarm_tm.tm_hour = alarm_time[2];
		alarm_tm.tm_mday = alarm_time[3];
		alarm_tm.tm_mon  = alarm_time[4]-1;
		alarm_tm.tm_year = alarm_time[5]-1900;
		*/
		alarm_tm = parse_alarm(alarm_time);
		*end = mktime(&alarm_tm);
		if (*end != 0)
			printf("Timer set for %s", ctime(end));
	}

	if (*end) {
		int diff = *end - cur;
		int abs_diff = (diff<0 ? diff*-1 : diff);
		int tcnt;
		char buff[5];
		unsigned int flags = 0;
		if (abs_diff > 60*60*60) /* use days, hrs, maybe mins - not secs */
			flags = T_DAY | T_HRS | (abs_diff%(60*60*60)>1 ? 0 : T_MIN);
		else if (abs_diff > 60*60) /* use hrs, mins, maybe secs */
			flags = T_HRS | T_MIN | (abs_diff%(60*60)>2 ? 0 : T_SEC);
		else if (abs_diff > 60) /* use mins and secs */
			flags = T_MIN | T_SEC;
		else flags = T_SEC;

		if ( flags & T_DAY ) {
			tcnt = diff/(60*60*60);
			diff -= tcnt*60*60*60;
			snprintf(buff, 5, "%id", tcnt);
			strncat(out, buff, 5);
		}
		if ( flags & T_HRS ) {
			tcnt = diff/(60*60);
			diff -= tcnt*60*60;
			snprintf(buff, 5, "%ih", tcnt);
			strncat(out, buff, 5);
		}
		if ( flags & T_MIN ) {
			tcnt = diff/60;
			diff -= tcnt*60;
			snprintf(buff, 5, "%im", tcnt);
			strncat(out, buff, 5);
		}
		if ( flags & T_SEC ) {
			snprintf(buff, 5, "%ih", diff);
			strncat(out, buff, 5);
		}
	} else {
		snprintf(out, 256, "UNSET");
	}
	return bprintf("%s", out);
}

void
alrm_dest(void)
{
	if(end)
		free(end);
}
