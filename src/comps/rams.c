/* See LICENSE file for copyright and license details. */
#include <stdio.h>

#include "../util.h"

const char *
getramf(void)
{
	long free;

	return (pscanf("/proc/meminfo", "MemFree: %ld kB\n", &free) == 1) ?
	       bprintf("%f", (float)free / 1024 / 1024) : NULL;
}

const char *
getramp(void)
{
	long total, free, buffers, cached;

	return (pscanf("/proc/meminfo",
	               "MemTotal: %ld kB\n"
	               "MemFree: %ld kB\n"
	               "MemAvailable: %ld kB\nBuffers: %ld kB\n"
	               "Cached: %ld kB\n",
	               &total, &free, &buffers, &buffers, &cached) == 5) ?
	       bprintf("%d", 100 * ((total - free) - (buffers + cached)) / total) :
	       NULL;
}

const char *
getramt(void)
{
	long total;

	return (pscanf("/proc/meminfo", "MemTotal: %ld kB\n", &total) == 1) ?
	       bprintf("%f", (float)total / 1024 / 1024) : NULL;
}

const char *
getramu(void)
{
	long total, free, buffers, cached;

	return (pscanf("/proc/meminfo",
	               "MemTotal: %ld kB\n"
	               "MemFree: %ld kB\n"
	               "MemAvailable: %ld kB\nBuffers: %ld kB\n"
	               "Cached: %ld kB\n",
	               &total, &free, &buffers, &buffers, &cached) == 5) ?
	       bprintf("%f", (float)(total - free - buffers - cached) / 1024 / 1024) :
	       NULL;
}
