# Makefile for SDL Sasteroids
# Note: I'm working on getting this working with autoconf/automake
# but haven't had time to do it yet. for now, this works for me.

CC = g++
INCLUDE = -I/usr/include
OPTIMIZE = -fomit-frame-pointer -O2 -ffast-math -funroll-loops -march=pentium -mcpu=pentium
OPTS = -Wall -pipe
GDIR=\"$(GAMEDIR)\"
#GDIR=\"/usr/share/sasteroids/\"
DEFINES = -DGAMEDIR=$(GDIR)


# for debugging
#CFLAGS	 = -Wall $(INC) $(DEBUG) $(OPTS) 
#DEBUG = -g -pg

#LFLAGS   = -static

SASTEROIDS = sasteroids
SASTEROIDS_O = fastmath.o obj.o sasteroids.o sbitmap.o ui.o ships.o

LIBS = `sdl-config --libs` -lSDL_ttf -lfreetype -lSDL_mixer

$(SASTEROIDS): $(SASTEROIDS_O)
	g++ -Wall -o $(SASTEROIDS) $(SASTEROIDS_O) $(DEFINES) $(DEBUG) $(LIBS)


.cpp.o:
	$(CC) -Wall $(DEFINES) $(CFLAGS) -c -o $*.o $<


.PHONY:	dep depend clean cleanall

cleanall: clean
	rm -f $(GAMELIB)

clean:
	rm -f *.o *~ sasteroids

#include dependancies



