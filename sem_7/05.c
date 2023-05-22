#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
 
int main()
{
    int pfd;
    int fd[2];
    size_t size, size1;
    char a[100];
    int ink = 0;
    if(pipe(fd) < 0)
    {
        printf("Error to create pipe");
        exit(-1);
    }
    else
    {
        while (size != size1) {
            size = write(fd[1], "a", 2);
            size1 = read(fd[0], a, 1);

            ++ink;
            printf("%d\n",ink);
        };
        
    }
    return 0;
}