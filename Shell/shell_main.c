/*  shell_main.c
 *  
 *  Ankit Gola:                 2017EET2296
 *  21.01.2018
 *
 *  This is the main file for the shell.
 *  It calls the shell loop function "sh_loop()" with no arguments.
 *  "sh_loop()" returns nothing.
 *  Main returns 0 in case of successful execution.
 *
 */

// Include files
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

// Header file
#include "shell.h"

#define clear() printf("\033[H\033[J")

/*
char *builtinArr[] = {"cd", "exit"};
int sh_builtinCount() {
    return (sizeof(builtinArr)/sizeof(char*));
}
*/
int main(int argc, char *argv[]) { 
//    printf("count: %d\n", sh_builtinCount());
//    printf("%s\n%s\n", builtinArr[0], builtinArr[1]);

//    signal(SIGINT, INThandler);
    signal(SIGINT, INThandler);
    clear();
    char *username = getenv("USER");
    struct utsname unameData;
    if(uname(&unameData)) {
        fprintf(stderr, "sh Error: Failed to get OS name\n");
    }

    //printf("%s\n", unameData.sysname);
    if(strcmp(unameData.sysname, "Linux")) {
        fprintf(stderr, "sh Error: This shell is made for Linux\n");
        return 0;
    } else {
        fprintf(stdout, "Welcome %s\n", username);
    }

    // Run shell loop
    sh_loop();

    return 0;
}
