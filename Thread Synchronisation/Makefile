CC = gcc

CFLAGS = -pthread -Wall -g

all: sync_main.c sync.c
	@echo "Building all.."
	${CC} ${CFLAGS} sync_main.c sync.c -o sync
