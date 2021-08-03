#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>

#define ERROR -1


void touch(const char* file){
    int result;
    int fd = open(file,O_RDONLY | O_CREAT,S_IRUSR|S_IWUSR);
    if (fd < 0 && errno != EACCES){
        fprintf(stderr, "Error opening %s: %s\n", file,strerror(errno));
        return;
    }

    result = utime(file, NULL);
    if (result < 0){
        fprintf(stderr, "Error updating the time: %s\n", strerror(errno));
    }

    result = close(fd);
    if (result < 0){
        fprintf(stderr, "Error closing the file: %s\n", strerror(errno));
    }
}

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr,"Not enough arguments, use ./touch file-to-be-created.\n");
        return ERROR;
    }

    for (int i = 1; i < argc; i++) {
        touch(argv[i]);
    }

    return 0;
}

