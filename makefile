status : status.o batt.o
	gcc status.o comps/batt.o comps/mopd.o comps/nets.o comps/time.o comps/volm.o -o status -lasound -lmpdclient -lX11

status.o : status.c status.h config.h bat.h
	gcc -c status.c

comps/bat.h : configure
	./configure

comps/batt.o : batt.c bat.h
	gcc -c batt.c

comps/mopd.o : mopd.c


comps/nets.o : nets.c


comps/time.o : time.c


comps/volm.o : volm.c


config.h : config.def.h
	cp config.def.h config.h

clean : 
	rm *.o */*.o status comps/bat.h
