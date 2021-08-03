#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#define ERROR -1


void mv(const char* old_path,const char* new_path){

    int result = rename(old_path,new_path);
    if (result < 0){
        fprintf(stderr, "Error renaming file from %s to %s: %s\n", old_path, new_path, strerror(errno));
    }

}

int main(int argc, char *argv[]) {

    if(argc < 3){
        fprintf(stderr,"Not enough arguments, use ./touch file-to-be-created.\n");
        return ERROR;
    }

    mv(argv[1],argv[2]);

    return 0;
}

