# Makefile for sasteroids

# TODO: Add System options.....

all: src/sasteroids

install: src/sasteroids
	./installbin

src/sasteroids:
	make -C src

clean:
	make -C src clean
