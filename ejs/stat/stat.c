#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#define ERROR -1


void stat0(const char* file){
    struct stat statbuf = {};
    int result = stat(file,&statbuf);
    if (result < 0){
        fprintf(stderr, "Error getting stats of %s: %s\n", file,strerror(errno));
        return;
    }
    char* type = S_ISREG(statbuf.st_mode) ? "Regular file" : "Directory";
    fprintf(stdout,"File: %s\n",file);
    fprintf(stdout,"Size: %ld\tBlocks: %ld\tIO Inode: %ld\tType: %s\n",
            statbuf.st_size,statbuf.st_blocks,statbuf.st_ino,type);

}

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr,"Not enough arguments, use ./touch file-to-be-created.\n");
        return ERROR;
    }

    for (int i = 1; i < argc; i++) {
        stat0(argv[i]);
    }

    return 0;
}

