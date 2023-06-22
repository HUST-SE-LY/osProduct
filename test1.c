#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 32

int main() {
    int fd;
    char buffer[BUFFER_SIZE]={};

    fd = open("/dev/hello",O_RDWR);
    if (fd < 0) {
        perror("an error occurred!");
        return -1;
    }

    ssize_t ret = read(fd, buffer, 16);
        
    printf("Read 16 bytes: %s\n", buffer);

    while(ret == 0) {
        ret = read(fd, buffer, 16);
        
        printf("Read 16 bytes: %s\n", buffer);
    }
    
    close(fd);
    return 0;
}
