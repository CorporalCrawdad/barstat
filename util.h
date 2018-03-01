extern char buf[1024];

#define LEN(x) (sizeof (x) / sizeof *(x))

const char *bprintf(const char *fmt, ...);
