// note: item width entails getfunc return and suffix. Excludes prefix
// note: set width to 0 for variadic width
baritem items[] = {
	/* function	prefix phrase	item width	suffix phrase */
	{ getsong,		"♫[ ",	0,		"]    "  },
	{ getvolm,		"🔈[",	3,		"%]    " },
	{ getbatlvl,		"🔋[",	3,		"%]    " },
	{ getnetu,		"网[",	5,		"上"   },
	{ getnetd,		" ",		5,		"下]"  },
	{ gettime,		"时[",		21,		"]"  },
};

const char* net_pref = "wlp";
