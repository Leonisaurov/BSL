CC			= gcc
CFLAGS		= -fPIC -c
AR			= ar
ARFLAGS		= rcs
RM = rm -fv

LIB_OBJS	= strings.o
LIB_NAME	= libstrings.so

BINARY		= library_test

%.o: %.c
	${CC} ${CFLAGS} $< -o $@

${LIB_NAME}: ${LIB_OBJS}
	${AR} ${ARFLAGS} $@ ${LIB_OBJS}

${BINARY}: ${LIB_NAME}
	${CC} main.c -L. -lstrings -o $@

test: ${BINARY}
	./${BINARY}

clean:
	${RM} ${BINARY} ${LIB_NAME} ${LIB_OBJS}

.PHONY: test clean
