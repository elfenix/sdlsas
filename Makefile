# Makefile for sasteroids

# Edit Bellow to change the installation locations
GAMEDIR=/usr/share/sasteroids/
BINDIR=/usr/bin/
MANDIR=/usr/man

all: src/sasteroids

install: src/sasteroids
	install -d $(GAMEDIR)
	install -d $(GAMEDIR)/graphics
	install -d $(GAMEDIR)/sounds
	install -d $(GAMEDIR)/fonts
	install -d $(BINDIR)
	install --strip --mode=755 src/sasteroids $(BINDIR)
	install --mode=644 sasteroids.6 $(MANDIR)/man6
	install --mode=644 fonts/Wargames.ttf $(GAMEDIR)/fonts/
	install --mode=644 sounds/*.wav $(GAMEDIR)/sounds/
	install --mode=644 graphics/*.bmp $(GAMEDIR)/graphics
	install --mode=644 graphics/sprite/*.bmp $(GAMEDIR)/graphics

src/sasteroids:
	make GAMEDIR=$(GAMEDIR) -C src

clean:
	make -C src clean
