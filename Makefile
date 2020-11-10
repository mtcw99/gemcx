NAME = gemcx

define \n

endef

define SRC
src/main.c
src/gemini/client.c
src/gemini/header.c
src/gemini/parser.c
src/ui/xcb/context.c
src/ui/xcb/key.c
src/ui/xcb/window.c
src/util/memory.c
endef

SRC := $(strip ${SRC})

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -Os -g
INCLUDEDIR = include/
LIBS = -D_POSIX_C_SOURCE=200809L -lssl -lcrypto -lxcb -lxkbcommon -lxkbcommon-x11
VERSION = 0.1
PREFIX = /usr/local
DISTDIR = ${NAME}-${VERSION}

${NAME}: ${SRC}
	@mkdir -p build/
	@${CC} -o build/${NAME} ${CFLAGS} ${SRC} -I${INCLUDEDIR} ${LIBS}
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

