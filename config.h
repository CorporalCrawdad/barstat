#include "status.h"

#define STAT_LEN  160 // make sure that all item widths + prefix phrases don't exceed this define
#define DEBUG
// failure to do so will segfault

// note: item width entails getfunc return and suffix. Excludes prefix
// note: set width to 0 for variadic width
baritem items[] = {
	/* function	prefix phrase	item width	suffix phrase */
	{ getsong,		"[MPD: ",	0,		"]"  },
	{ getvol,		"[Vol: ",	5,		"%]" },
	{ getbat,		"[BAT: ",	5,		"%]" },
	{ getnetu,		"[NET: ",	5,		""   },
	{ getnetd,		"|",		6,		"]"  },
	{ gettime,		"[",		21,		"]"  },
};

const char* net_pref = "wlp";
