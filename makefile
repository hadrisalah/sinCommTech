CC= gcc
GTKCFLGS= `pkg-config gtk+-3.0 --cflags`
GTKLIBS= `pkg-config gtk+-3.0 --libs`
XMLCFLAGS= `pkg-config libxml-2.0 --cflags`
XMLLIBS= `pkg-config libxml-2.0 --libs`
SDLCFLAGS=`pkg-config sdl2 --cflags`
SDLLIBS=`pkg-config sdl2 --libs`
PTHREADSLIBS= -lpthread
PTHREADCONST= -D_REENTRANT -DLinux
SPECIALWARNING= -Wimplicit-function-declaration

all: commServer

commServer: commServer.c sinliste.o sinnet.o sinxml.o sinsentinel.o
	$(CC) commServer.c sinnet.o sinliste.o sinxml.o sinsentinel.o -o commServer $(PTHREADSLIBS) $(PTHREADCONST) $(XMLCFLAGS) $(XMLLIBS)

sinnet.o: sinnet.c
	$(CC) -c sinnet.c -o sinnet.o

sinliste.o: sinliste.c
	$(CC) -c sinliste.c -o sinliste.o

sinxml.o: sinxml.c
	$(CC) -c sinxml.c $(XMLCFLAGS) $(XMLLIBS) -o sinxml.o

sinsentinel.o: sinsentinel.c sinliste.o sinnet.o
	$(CC) -c sinsentinel.c $(XMLCFLAGS) $(XMLLIBS) -o sinsentinel.o

clean: 
	rm -rf *.o
