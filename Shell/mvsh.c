/*  mvsh.c
 *
 *  Ankit Gola:                 2017EET2296
 *
 *  This program moves the files, deleting the source file.
 *  USAGE: mvsh <srcfile path> <dest file path>
 *  
 *  NOTE: src and dest must be on same filesystem
 *
 */

// Include files
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    // Check for proper arguments
    if(argc < 3 || !(argc%2)) {
        fprintf(stderr, "mvsh Error: Incomplete Arguments.\nPlease provide both source and destination path for all the files\n");
        fprintf(stdout, "USAGE: ./mvsh <src file path> <dest file path> ... ...\n");
        return 0;
    }

    int i = 1;
    // Loop for all args 
    while(i < argc) {
        //printf("Srf: %s\n", argv[i]);
        //printf("Dest: %s\n", argv[i+1]);
        
        // Check src file existence and permissions
        if(access(argv[i], F_OK|W_OK) == -1) {
            fprintf(stderr, "mvsh Error: Src file \"%s\" does not exist\n", argv[i]);
            return 0;
        }

        // Parse the destination directory
        char tempDir[PATH_MAX];
        strcpy(tempDir, argv[i+1]);
        int ch = '/';
        char *ptr = strrchr(tempDir, ch);
        if(ptr) {
            tempDir[ptr - tempDir + 1] = '\0';
        } else {
            getcwd(tempDir, PATH_MAX);
        }
        //printf("Dest Dir: %s\n", tempDir);
        //printf("Dest full: %s\n", argv[i+1]);
       
        // Dest Folder existence and permissions
        if(access(tempDir, F_OK) != 0) {
           fprintf(stderr, "mvsh Error: Dest folder doesn't exist\n");
           return 0;
        }
        if(access(tempDir, W_OK) != 0) {
           fprintf(stderr, "mvsh Error: Dest folder is write protected\n");
           return 0;
        }
       
       // Check existence of src file in dest, and its permissions
        if(access(argv[i+1], F_OK|W_OK) != -1) {
            fprintf(stderr, "Dest \"%s\" exists. Not replcaing file.\n", argv[i+1]);
            return 0;
        }

        // Time the move
        clock_t t;
        t = clock();
       // char rename[3 * PATH_MAX] = "rename 's/";
       // char sysCall[3 * PATH_MAX] = "mv ";
       // strcat(sysCall, argv[i]);
       // strcat(sysCall, " ");
       // strcat(sysCall, argv[i+1]);
       // strcat(rename, "/;' *");
       // if(system(sysCall)) {
        if(rename(argv[i], argv[i+1])) {
            fprintf(stderr, "mvsh Error: Move failure\n");
        }
        t = clock() - t;
        double time = ((double)t)/CLOCKS_PER_SEC * 1000000;
        printf("Moving \"%s\" took %f nsec\n", argv[i], time);
        i += 2;
    }
    return 1;
}
