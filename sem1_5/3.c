#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
    pid_t pid;

    pid = fork();

    if (pid < 0) {
            return 0;
    }
    else if (pid == 0) {
            printf("Child\n");
            (void) execle("/bin/cat", "/bin/cat", "3.c", 0, envp);
    }
    else {
            printf("Parent\n");
    }
    return 0;

}