#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>

#define str "file"

int main() {

    int fd;

    if((fd = open(str, O_RDWR | O_EXCL | O_CREAT, 0777)) < 0) {

        fprintf(stderr, "Failed to create file\n");
        exit(EXIT_FAILURE);
    }

    close(fd);

    char buff[30] = {};
    char buff_p[30] = {};
    memcpy(buff_p, str, sizeof(str));

    int i = 0;

    while(true) {

        sprintf(buff, str "_%d", i);
        int ln = symlink(buff_p, buff);
        memcpy(buff_p, buff, 30);

        if(ln <  0) {
            fprintf(stderr, "Failed to create symlink;\nbuff = %s\nfile = %s\n", buff, str);
            exit(EXIT_FAILURE);
        }
        
        fd = open(buff, O_APPEND | O_WRONLY);
        dprintf(fd, "test_%d\n", i);

        if(fd < 0) {
            fprintf(stdout, "Failed open symlink; i = %d\n", i);
            perror("Symlink error");
            break;
        }

        close(fd);

        i++;
        if(i > 100000)
            break;
    }


    return 0;
}