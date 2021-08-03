#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define ERROR -1


void rm(const int argc, char *argv[]) {
    int result;
    for (int i = 1; i < argc ; i++) {
        result = unlink(argv[i]);
        if (result < 0) {
            fprintf(stderr, "Error removing %s: %s\n", argv[i],strerror(errno));
        }
        else{
            fprintf(stdout,"Removed %s\n",argv[i]);
        }
    }
}

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr,"Not enough arguments, use ./rm file-to-be-removed.\n");
        return ERROR;
    }

    rm(argc,argv);
    return 0;
}