#Makefile

CC = gcc

CFLAGS = -Wall -g

sh : shell_main.c shell.c
	@echo "Building sh.."
	${CC} ${CFLAGS} shell_main.c shell.c stack.c -o sh

mvsh : mvsh.c
	@echo "Building mvsh.."
	${CC} ${CFLAGS} mvsh.c -o mvsh

mksh : mksh.c
	@echo "Building mksh.."
	${CC} ${CFLAGS} mksh.c -o mksh

all : shell_main.c shell.c mksh.c mvsh.c
	@echo "Building all.. "
	${CC} ${CFLAGS} shell_main.c shell.c stack.c -o sh
	${CC} ${CFLAGS} mvsh.c -o mvsh
	${CC} ${CFLAGS} mksh.c -o mksh

EXECS: sh mvsh mksh

clean :
	@echo "Cleaning up.."
	rm -rf ${EXECS}
	rm -rf *.o
	rm -rf rand*
