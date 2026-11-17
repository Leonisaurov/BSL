CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic -Wformat=2 -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wundef -Wpointer-arith -fstack-protector-strong
CPPFLAGS ?= -D_FORTIFY_SOURCE=2
LDFLAGS ?=
RM = rm -fv

UNAME_S := $(shell uname -s)
HARDEN_LDFLAGS =
APP_LDFLAGS =
SHARED_LDFLAGS =
ifeq (${UNAME_S},Linux)
HARDEN_LDFLAGS = -Wl,-z,relro,-z,now -Wl,-z,noexecstack
APP_LDFLAGS = -pie
SHARED_LDFLAGS = -Wl,-soname,libstrings.so
endif

LIB_OBJS = strings.o animations.o
LIB_NAME = libstrings.so

APP_SRC = main.c
APP = library_test

.PHONY: all test clean

all: ${APP}

%.o: %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -fPIC -c $< -o $@

${LIB_NAME}: ${LIB_OBJS}
	${CC} -shared ${SHARED_LDFLAGS} -o $@ ${LIB_OBJS} ${LDFLAGS} ${HARDEN_LDFLAGS}

${APP}: ${LIB_NAME} ${APP_SRC}
	${CC} ${CPPFLAGS} ${CFLAGS} ${APP_SRC} -L. -lstrings ${APP_LDFLAGS} ${LDFLAGS} ${HARDEN_LDFLAGS} -o $@

test: ${APP}
	LD_LIBRARY_PATH=. ./${APP}

clean:
	${RM} ${APP} ${LIB_NAME} ${LIB_OBJS}