
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ERROR -1
#define MAX_BUF 256

void write_to_stdout(int bytes_read, char buffer[MAX_BUF], const char *file){
    int bytes_written = write(STDOUT_FILENO, buffer,bytes_read);

    while (0 < bytes_written && bytes_written < bytes_read){ //The number of bytes written may be less than bytes_read
        bytes_read = bytes_read - bytes_written;
        strncpy(buffer,buffer + bytes_written,bytes_read);
        bytes_written = write(STDOUT_FILENO, buffer,bytes_read);
    }

    if (bytes_written < 0){
        fprintf(stderr, "Error concatenating %s: %s\n", file,strerror(errno));
    }
}

int cat(const char *file) {

    char buffer[MAX_BUF] = {};
    int result, bytes_read = 0;

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", file, strerror(errno));
        return ERROR;
    }

    bytes_read = read(fd, buffer, MAX_BUF);
    while (0 < bytes_read){
        write_to_stdout(bytes_read, buffer, file);
        strcpy(buffer,"");
        bytes_read = read(fd,buffer,MAX_BUF);
    }

    if (bytes_read < 0){
        fprintf(stderr, "Error reading %s: %s\n", file, strerror(errno));
    }

    result = close(fd);
    if (result < 0){
        fprintf(stderr, "Error closing %s: %s\n", file, strerror(errno));
    }
    return 0;
}

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr,"Not enough arguments, use ./cat file-to-be-concatenated.\n");
        return ERROR;
    }

    for (int i = 1; i < argc; i++) {
        cat(argv[i]);
    }

    return 0;
}