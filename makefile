status : status.o 
	gcc status.o -o status -lasound -lmpdclient -lX11

status.o : status.c status.h config.h bat.h
	gcc -c status.c

bat.h : configure
	./configure

config.h : config.def.h
	cp config.def.h config.h

clean : 
	rm *.o status bat.h
