// note: item width entails getfunc return and suffix. Excludes prefix
// note: set width to 0 for variadic width
baritem items[] = {
	/* function	prefix phrase	item width	suffix phrase */
	{ getsong,		"[♫",		0,		"]"  },
	{ getvolm,		"[🔊",		5,		"%]" },
//	{ getbatlvl,		"[B",		5,		"%]" },
	{ getnetu,		"[🖧",		5,		"↾"   },
	{ getnetd,		" ",		6,		"⇂]"  },
	{ gettime,		"[⏲",		21,		"]"  },
};

const char net_pref[4] = "enp";
