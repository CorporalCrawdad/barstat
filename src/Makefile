include config.mk
all: clean barstat


COMPS= \
       comps/alrm\
       comps/batt\
       comps/mopd\
       comps/nets\
       comps/pipe\
       comps/rams\
       comps/time\
       comps/vmst\
       comps/volm

util.o : util.c util.h
	$(CXX) -c util.c

barstat.o : barstat.c status.h config.h
	$(CXX) -c barstat.c

comps/bat.h : configure
	./configure

$(COMPS:=.o) : $(@:.o=.c) util.o comps/bat.h
	$(CXX) -o $@ -c $(@:.o=.c)

config.h : config.def.h
	cp config.def.h config.h -n

barstat : barstat.o $(COMPS:=.o) util.o
	$(CXX) barstat.o $(COMPS:=.o) util.o -o ../barstat -lasound -lmpdclient -lX11 -lvirt -lm

debug : barstat.o $(COMPS:=.o) util.o
	$(CXX) barstat.o $(COMPS:=.o) util.o -o ../barstat -lasound -lmpdclient -lX11 -lvirt -lm -g

clean : 
	rm */*.o *.o ../barstat comps/bat.h -f

install: barstat
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ../barstat ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/barstat

