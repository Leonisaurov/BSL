CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?=
LDFLAGS ?=
RM = rm -fv

LIB_OBJS = strings.o animations.o
LIB_NAME = libstrings.so

APP_SRC = main.c
APP = library_test

.PHONY: all test clean

all: ${APP}

%.o: %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -fPIC -c $< -o $@

${LIB_NAME}: ${LIB_OBJS}
	${CC} -shared -o $@ ${LIB_OBJS} ${LDFLAGS}

${APP}: ${LIB_NAME} ${APP_SRC}
	${CC} ${CPPFLAGS} ${CFLAGS} ${APP_SRC} -L. -lstrings -Wl,-rpath,'$$ORIGIN' ${LDFLAGS} -o $@

test: ${APP}
	./${APP}

clean:
	${RM} ${APP} ${LIB_NAME} ${LIB_OBJS}
