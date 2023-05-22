#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {

    if (argc != 2) {
        printf("Wrong argument count\n");
        exit(-1);
    }

    size_t size = 0;

    int fd = atoi(argv[1]);
    size = write(fd, "Its work!", 14);
    if(size != 14){
        printf("Can\'t write all string to pipe\n");
        exit(-1);
    }

    return 0;
}