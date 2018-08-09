/*  mksh.c
 *
 *  Ankit Gola:                 2017EET2296
 *
 *  This program makes a file of given size in parent directory.
 *  USAGE: mksh <filesize> <filename> .. 
 *         mksh -d : run mksh with default parameters
 */

// Include Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
/*    if(argv[1] && !strcmp(argv[1], "-d")) {
        if(system("fallocate -l 1M rand1M") || system("fallocate -l 10M rand10M") || system("fallocate -l 100M rand100M")) {
            fprintf(stderr, "mksh Error: Fail to create default files\n");
            return 0;
        }
        return 1;
    }
*/
    // Default, makes three required files
    if(argv[1] && !strcmp(argv[1], "-d")) {
        char *fname[] = {"rand1M", "rand10M", "rand100M"};
        int mul = 1024*1024;
        int i = 0;
        int len = 1;
        while(i < 3) {
            // Check for file permissions
            int fd = open(fname[i], O_RDWR|O_CREAT, 0666);
            if(posix_fallocate(fd, 0, len*mul)) {
                fprintf(stderr, "mksh Error: Cannot create \"%s\" file", fname[i]);
                return 0;
            }
            i++;
            len *= 10;
        }
        return 1;
    }

    // If number of arguments less than 3 or numebr of arguments is not an odd number.
    if(argc < 3 || !(argc%2)) {
        fprintf(stderr, "mksh Error: Incomplete Arguments\nUSAGE: ./mksh <filesize> <filename> ...\n");
        return 0;
    }

    int i = 1;
    // Loop for all files in args
    while(argv[i]) {
//        char sysCall[64] = "fallocate -l ";
//        strcat(sysCall, argv[i]);
//        strcat(sysCall, " ");
//        strcat(sysCall, argv[i+1]);
//        if(system(sysCall)) {
//            fprintf(stderr, "mksh Error: Failed to create file \"%s\" of size \"%s\"\n", argv[i+1], argv[i]);
//        }

        // Parse args
        int fd = open(argv[i+1], O_RDWR|O_CREAT, 0666);
        char size[8];
        strcpy(size, argv[i]);
        int sz = strlen(size);
        //printf("%c\n", size[sz-1]);
        long long int mul = 1;

        // Adjust size of file according to size
        if(size[sz-1] == 'M') {
            mul = 1024 * 1024;
        } else if(size[sz-1] == 'G') {
            mul = 1024 * 1024 * 1024;
//        } else if(size[sz-1] == 'T') {
//            mul = 1024 * 1024 * 1024 * 1024;
        }
        size[sz-1] = '\0';
        int len = atoi(size);
       
        // Create file of given size
        if(posix_fallocate(fd, 0, len * mul)) {
            fprintf(stderr, "mksh Error: Cannot create \"%s\" file\n", argv[i+1]);
        }
        i += 2;
    }

/*
    int fd = ("rand1, o_RDWR|O_CREAT, 0666);
    int len = 1024*1024;
    posix_fallocate(fd, 0, len);
*/
    return 0;
}
