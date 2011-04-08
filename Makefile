# wimp - WIndow Manager Profil
# makefile

DESTDIR = /usr

all:
	gcc -Wall -ansi -pedantic main.c -o wimp -lX11

install: all
	cp -f wimp ${DESTDIR}/bin
	chmod 755 ${DESTDIR}/bin/wimp

uninstall:
	rm ${DESTDIR}/bin/wimp

clean:
	rm wimp