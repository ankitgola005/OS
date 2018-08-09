#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <string.h>
#include <unistd.h>

//#define _BSD_SOURCE
extern int alphasort();

int file_select(struct direct *entry) {
    if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        return 0;
    else
        return 1;
}

int main() {
   // setenv("CWD", "/home/prog/c", 1);
    struct direct **files;
    char pathname[4096];
    getcwd(pathname, sizeof(pathname));
    int count = scandir(pathname, &files, file_select, alphasort);
    for(int i = 0; i < count; i++) {
        printf("%s\n", files[i]->d_name);
    }
    return 0;
}
