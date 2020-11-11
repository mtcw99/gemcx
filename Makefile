NAME = gemcx

define \n

endef

define SRC
src/main.c
src/gemini/client.c
src/gemini/header.c
src/gemini/parser.c
src/gopher/client.c
src/gopher/parser.c
src/ui/xcb/context.c
src/ui/xcb/key.c
src/ui/xcb/window.c
src/ui/xcb/event.c
src/ui/xcb/text.c
src/ui/xcb/pixmap.c
src/util/memory.c
src/util/socket.c
endef

SRC := $(strip ${SRC})

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -Os -g
#CFLAGS = -std=c99 -pedantic -Wall -Os -flto
INCLUDEDIR = include/
LIBS = -D_POSIX_C_SOURCE=200809L -lssl -lcrypto -lxcb -lxkbcommon -lxkbcommon-x11
PKG = pangocairo fontconfig
PKGCFG = `pkg-config --libs --cflags ${PKG}`
VERSION = 0.1
PREFIX = /usr/local
DISTDIR = ${NAME}-${VERSION}

build/${NAME}: ${SRC}
	@mkdir -p build/
	@${CC} -o build/${NAME} ${CFLAGS} ${SRC} -I${INCLUDEDIR} ${PKGCFG} ${LIBS}
	@echo "executable in build/${NAME}"

clean:
	@echo cleaning
	@rm -fr build/ ${DISTDIR}.tar.gz

dist:
	@echo creating dist tarball
	@mkdir -p ${DISTDIR}
	@cp -R LICENSE Makefile README.md ${SRC} ${DISTDIR}
	@tar -cf - "${DISTDIR}" | \
		gzip -c > "${DISTDIR}.tar.gz"
	@rm -rf "${DISTDIR}"

.PHONY: clean dist

