# Makefile for sasteroids

# Edit Bellow to change the installation locations
GAMEDIR=/usr/share/sasteroids/
BINDIR=/usr/bin/
MANDIR=/usr/man

# For OpenGL
EXTRA_DEFINES="-DWANT_OPENGL -DHAVE_SOUND"
EXTRA_LIBS=-lGL

# For SDL_gfx - NOT SUPPORTED AT THIS TIME (will work, some features missing...)
# EXTRA_DEFINES=-DHAVE_SOUND
# EXTRA_LIBS=-lSDL_gfx


all: 
	make GAMEDIR=$(GAMEDIR) EXTRA_DEFINES=$(EXTRA_DEFINES) EXTRA_LIBS=$(EXTRA_LIBS) -C src

install: 
	make GAMEDIR=$(GAMEDIR) -C src
	install -d $(GAMEDIR)
	install -d $(GAMEDIR)/graphics
	install -d $(GAMEDIR)/sounds
	install -d $(GAMEDIR)/fonts
	install -d $(BINDIR)
	install --strip --mode=755 src/sasteroids $(BINDIR)
	install --mode=644 sasteroids.6 $(MANDIR)/man6
	install --mode=644 fonts/nicefont.ttf $(GAMEDIR)/fonts/
	install --mode=644 sounds/*.wav $(GAMEDIR)/sounds/
	install --mode=644 graphics/*.png $(GAMEDIR)/graphics
	install --mode=644 graphics/sprite/*.png $(GAMEDIR)/graphics
	install --mode=644 graphics/backdrops/*.jpg $(GAMEDIR)/graphics

	make GAMEDIR=$(GAMEDIR) -C src

clean:
	make -C src clean
