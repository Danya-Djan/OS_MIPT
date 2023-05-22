#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define PIPE_NAME "my_pipe"
#define SEM_KEY 1234
#define BUFFER_SIZE 256

struct semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    int sem_id;
    pid_t pid;

    // Create a named pipe
    mkfifo(PIPE_NAME, 0666);

    // Create a semaphore
    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);

    // Initialize the semaphore
    union semun arg;
    arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }


    // Fork a child process
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Child process: write and read data to/from the pipe in a loop
        fd = open(PIPE_NAME, O_WRONLY);

        while (1) {
            printf("Child process waiting for permission to write to pipe...\n");
            struct sembuf op[] = { { 0, -1, 0 } };
            semop(sem_id, op, 1);
            printf("Child process writing to pipe...\n");
            strcpy(buffer, "Hello, parent process!");
            write(fd, buffer, strlen(buffer) + 1);
            printf("Child process waiting for permission to read from pipe...\n");
            op[0].sem_op = 1;
            semop(sem_id, op, 1);
            read(fd, buffer, BUFFER_SIZE);
            printf("Child process received message from parent: %s\n", buffer);
        }

        close(fd);
        return 0;
    } else {
        // Parent process: read and write data to/from the pipe in a loop
        fd = open(PIPE_NAME, O_RDONLY);

        while (1) {
            printf("Parent process waiting for permission to read from pipe...\n");
            struct sembuf op[] = { { 0, -1, 0 } };
            semop(sem_id, op, 1);
            read(fd, buffer, BUFFER_SIZE);
            printf("Parent process received message from child: %s\n", buffer);
            printf("Parent process waiting for permission to write to pipe...\n");
            op[0].sem_op = 1;
            semop(sem_id, op, 1);
            printf("Parent process writing to pipe...\n");
            strcpy(buffer, "Hello, child process!");
            write(fd, buffer, strlen(buffer) + 1);
        }

        close(fd);
        wait(NULL);
        return 0;
    }
}
