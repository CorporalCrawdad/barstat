// note: item width entails getfunc return and suffix. Excludes prefix
// note: set width to 0 for variadic width
baritem items[] = {
	/* function	prefix phrase	item width	suffix phrase */
	{ getsong,		"[MPD: ",	0,		"]"  },
	{ getvolm,		"[Vol: ",	5,		"%]" },
	{ getbatlvl,		"[BAT: ",	5,		"%]" },
	{ getnetu,		"[NET: ",	5,		""   },
	{ getnetd,		"|",		6,		"]"  },
	{ gettime,		"[",		21,		"]"  },
};

const char* net_pref = "wlp";
