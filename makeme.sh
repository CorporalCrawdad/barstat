#!/bin/sh
gcc barstat.c util.c comps/batt.c comps/mopd.c comps/nets.c comps/time.c -o barstat -lmpdclient -lX11
