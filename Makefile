# Makefile for sasteroids

# Edit Bellow to change the installation locations
GAMEDIR=/usr/share/sasteroids/
BINDIR=/usr/bin/
MANDIR=/usr/man

all: 
	make GAMEDIR=$(GAMEDIR) -C src

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
	install --mode=644 graphics/backdrops/* $(GAMEDIR)/graphics

	make GAMEDIR=$(GAMEDIR) -C src

clean:
	make -C src clean
