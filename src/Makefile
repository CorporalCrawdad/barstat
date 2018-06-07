include config.mk

COMPS= comps/mopd\
       comps/batt\
       comps/nets\
       comps/time\
       comps/rams\
       comps/volm

util.o : util.c util.h
	gcc -c util.c

barstat.o : barstat.c status.h config.h
	gcc -c barstat.c

comps/bat.h : configure
	./configure

$(COMPS:=.o) : $(@:.o=.c) util.o comps/bat.h
	gcc -o $@ -c $(@:.o=.c)

config.h : config.def.h
	cp config.def.h config.h -n

barstat : barstat.o $(COMPS:=.o) util.o
	gcc barstat.o $(COMPS:=.o) util.o -o ../barstat -lasound -lmpdclient -lX11

debug : barstat.o $(COMPS:=.o) util.o
	gcc barstat.o $(COMPS:=.o) util.o -o ../barstat -lasound -lmpdclient -lX11 -g

clean : 
	rm */*.o *.o ../barstat comps/bat.h -f

all: clean barstat

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ../barstat ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/barstat
