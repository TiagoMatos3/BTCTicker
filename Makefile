CC = gcc
CFLAGS = -lcurl -lcjson -lsqlite3

btcticker: curls.c main.c
	${CC} ${CFLAGS} -o btcticker curls.c main.c
