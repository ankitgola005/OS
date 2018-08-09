#Makefile

CC = gcc

CFLAGS = -Wall -g

all: server_main.c server.c client_main.c client.c
	@echo "Building all.. "
	${CC} ${CFLAGS} server_main.c server.c common.c -o server
	${CC} ${CFLAGS} client_main.c client.c common.c -o client

client: client_main.c client.c common.c
	@echo "Building client.."
	${CC} ${CFLAGS} client_main.c client.c common.c -o client

server: server_main.c server.c common.c
	@echo "Building server.."
	${CC} ${CFLAGS} server_main.c server.c common.c -o server

#EXECS: sh mvsh mksh

#clean :
#	@echo "Cleaning up.."
#	rm -rf ${EXECS}
#	rm -rf *.o
#	rm -rf rand*
