status : status.o 
	gcc status.o -o status -lasound -lmpdclient -lX11

status.o : status.c status.h config.h bat.h
	gcc -c status.c

bat.h : configure
	./configure

clean : 
	rm *.o status bat.h
