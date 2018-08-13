/*  sync_main.c
 *
 *  Ankit Gola                  :       2017EET2296
 *  Pushpendra Singh Dahiya     :       2017EET2840
 *
 *  This is the main file for sync
 *
 */

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Program specific header file
#include "sync.h"

// Main
int main(int argc, char *argv[]) {
    // Seed rand with time
    srand(time(NULL));

    if(argc == 3) {
        int numUserInit = atoi(argv[1]);
        maxProcess = numUserInit;
        init(numUserInit);

        int numServInit = atoi(argv[2]);
        maxServicemen = numServInit;
    }
    else {
        maxProcess = 10;
        maxServicemen = 2;
    }
    printf(BOLD CYAN "Starting the service\n" RESET);

    // Initialise the pthread variables and attributes
    pthread_t t1d, t2d;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // spawn two threads: 
    // 1. Handle user input from command line
    // 2. Handle USER and servicemen
    pthread_create(&t1d, &attr, handle_cmd, NULL);
    pthread_create(&t2d, &attr, servicemen, NULL);
    
    // Wait for threads to exit
    pthread_join(t1d, NULL);
    pthread_join(t2d, NULL);

    printf(BOLD CYAN "Closing the Service\n" RESET);
    return 0;
}
