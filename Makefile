NAME = gemcx

define \n

endef

define SRC
src/main.c
src/protocol/client.c
src/protocol/historyStack.c
src/protocol/header.c
src/protocol/gemini/client.c
src/protocol/gemini/header.c
src/protocol/gopher/client.c
src/parser/generic.c
src/parser/format/unknown.c
src/parser/format/gemini.c
src/parser/format/gopher.c
src/parser/format/text.c
src/parser/format/image.c
src/render/xcb.c
src/render/links.c
src/render/format/gemini/xcb.c
src/render/format/gopher/xcb.c
src/render/format/text/xcb.c
src/ui/xcb/context.c
src/ui/xcb/key.c
src/ui/xcb/window.c
src/ui/xcb/windowShared.c
src/ui/xcb/subwindow.c
src/ui/xcb/event.c
src/ui/xcb/text.c
src/ui/xcb/pixmap.c
src/ui/xcb/button.c
src/ui/xcb/cursor.c
src/ui/xcb/monitors.c
src/ui/xcb/textInput.c
src/ui/xcb/clipboard.c
src/ui/xcb/menu.c
src/ui/xcb/image.c
src/util/memory.c
src/util/socket.c
src/util/stack.c
src/util/ex.c
src/gemcx/xcb/controlBar.c
src/gemcx/xcb/connectUrl.c
src/gemcx/xcb/globals.c
src/gemcx/xcb/eventResponse.c
src/gemcx/config.c
endef

SRC := $(strip ${SRC})

CC = cc
CFLAGS = -std=c99 -pedantic -Wall -D_POSIX_C_SOURCE=200809L 
CFLAGS_DEBUG = -O0 -g -DDEBUG
CFLAGS_RELEASE = -Os -flto
INCLUDEDIRS = -Iinclude/ -Ivendor/include/
LIBS = -lm -lssl -lcrypto -lxcb -lxcb-cursor -lxcb-icccm -lxcb-randr -lxcb-image
PKG = pangocairo fontconfig xkbcommon xkbcommon-x11
PKGCFG = `pkg-config --libs --cflags ${PKG}`
VERSION = 0.1
PREFIX = /usr/local
DISTDIR = ${NAME}-${VERSION}

build/${NAME}: ${SRC}
	@mkdir -p build/
	@${CC} -o build/${NAME} ${CFLAGS} ${CFLAGS_RELEASE} ${SRC} ${INCLUDEDIRS} ${PKGCFG} ${LIBS}
	@echo "release build: executable in build/${NAME}"

debug: ${SRC}
	@mkdir -p build/
	@${CC} -o build/${NAME} ${CFLAGS} ${CFLAGS_DEBUG} ${SRC} ${INCLUDEDIRS} ${PKGCFG} ${LIBS}
	@echo "debug build: executable in build/${NAME}"

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

.PHONY: clean dist debug

