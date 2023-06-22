#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 32
#define MESSAGE "helloworld"

int main() {
    int fd;
char s[10]={'h','e','l','l','o','w','o','r','l','d'};
    fd = open("/dev/hello", O_RDWR);
    if (fd < 0) {
        perror("an error occurred!");
        return -1;
    }
  
    ssize_t ret = write(fd, s,10);
    if (ret < 0) {
        perror("error occurred when writing");
    } else {
        printf("Written 10 bytes: helloworld\n"); 
    
    }

    while(ret == 0) {
        sleep(1);
        ret = write(fd, s,10);
        if (ret < 0) {
            perror("an error occurred when writing");
        } else {
            printf("Written 10 bytes: helloworld\n"); 
    
        }
    }
  
  
   close(fd);
    return 0;
}
