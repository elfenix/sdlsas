# Makefile for sasteroids

# Edit Bellow to change the installation locations
GAMEDIR=/usr/share/sasteroids/
BINDIR=/usr/bin/
MANDIR=/usr/man

all: src/sasteroids

install: src/sasteroids
	install -d $(GAMEDIR)
	install -d $(BINDIR)
	install --strip --mode=4755 src/sasteroids $(BINDIR)
	install --mode=644 sast.cf $(GAMEDIR)
	install --mode=644 sasteroids.6 $(MANDIR)/man6
	install --mode=644 fonts/Wargames.ttf $(GAMEDIR)
	install --mode=644 sounds/*.wav $(GAMEDIR)

src/sasteroids:
	make GAMEDIR=$(GAMEDIR) -C src

clean:
	make -C src clean
