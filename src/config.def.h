// note: item width entails getfunc return and suffix. Excludes prefix
// note: set width to 0 for variadic width
baritem items[] = {
	/* function	prefix phrase	item width	suffix phrase */
	{ getsong,		"♫[ ",		0,		"]    "  },
	{ getvolm,		"🔈[",		3,		"%]    " },
	{ getbatlvl,		"🔋[",		3,		"%]    " },
	{ getramu,		"内存[",	5,		"GB/" },
	{ getramf,		"",		5,		"GB] " },
	{ getramp,		"(",		3,		"%)    " },
	{ getnetu,		"网[",		5,		"上"   },
	{ getnetd,		" ",		5,		"下]    "  },
	{ gettime,		"时[",		21,		"]"  },
};

const char *net_pref    = "wlp";
const char *alsa_device = "default"; // if using other than default, it will be something of the form "hw:0"
