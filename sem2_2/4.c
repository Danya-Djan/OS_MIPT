#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>

#define PIPE_NAME "mypipe"
#define SEM_PARENT_NAME "/parent_sem"
#define SEM_CHILD_NAME "/child_sem"
#define BUFFER_SIZE 256

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    sem_t *parent_sem, *child_sem;
    pid_t pid;

    // Create a named pipe
    mkfifo(PIPE_NAME, 0666);

    // Create two semaphores
    parent_sem = sem_open(SEM_PARENT_NAME, O_CREAT, 0666, 0);
    child_sem = sem_open(SEM_CHILD_NAME, O_CREAT, 0666, 0);

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Child process: write and read data to/from the pipe in a loop
        fd = open(PIPE_NAME, O_WRONLY);

        while (1) {
            sem_wait(child_sem);
            printf("Child process writing to pipe...\n");
            strcpy(buffer, "Hello, parent process!");
            write(fd, buffer, strlen(buffer) + 1);
            sem_post(parent_sem);
            sem_wait(child_sem);
            printf("Child process waiting to read from pipe...\n");
            read(fd, buffer, BUFFER_SIZE);
            printf("Child process received message from parent: %s\n", buffer);
            sem_post(parent_sem);
        }

        close(fd);
        return 0;
    } else {
        // Parent process: read and write data to/from the pipe in a loop
        fd = open(PIPE_NAME, O_RDONLY);

        while (1) {
            sem_wait(parent_sem);
            printf("Parent process waiting to read from pipe...\n");
            read(fd, buffer, BUFFER_SIZE);
            printf("Parent process received message from child: %s\n", buffer);
            sem_post(child_sem);
            sem_wait(parent_sem);
            printf("Parent process writing to pipe...\n");
            strcpy(buffer, "Hello, child process!");
            write(fd, buffer, strlen(buffer) + 1);
            sem_post(child_sem);
        }

        close(fd);
        wait(NULL);
        return 0;
    }
}
