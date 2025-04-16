CC			= gcc
CFLAGS		= -fPIC -c
AR			= ar
ARFLAGS		= rcs
RM = rm -fv

LIB_OBJS	= strings.o
LIB_NAME	= libstrings.so

APP_SRC = main.c
APP		= library_test

%.o: %.c
	${CC} ${CFLAGS} $< -o $@

${LIB_NAME}: ${LIB_OBJS}
	${AR} ${ARFLAGS} $@ ${LIB_OBJS}

${APP}: ${LIB_NAME} ${APP_SRC}
	${CC} ${APP_SRC} -L. -lstrings -o $@

test: ${APP}
	./${APP}

clean:
	${RM} ${BINARY} ${LIB_NAME} ${LIB_OBJS}

.PHONY: test clean
