# Makefile for sasteroids

# TODO: Add System options.....

all: src/sasteroids

install: src/sasteroids
	install -d /usr/share/sasteroids
	install -d /usr/bin
	install --strip --mode=4755 src/sasteroids /usr/bin/
	install --mode=644 sast.cf /usr/share/sasteroids
	install --mode=644 sasteroids.6 /usr/man/man6
	install --mode=644 fonts/Wargames.ttf /usr/share/sasteroids
	install --mode=644 sounds/*.wav /usr/share/sasteroids

src/sasteroids:
	make -C src

clean:
	make -C src clean
