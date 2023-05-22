#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>

#define PIPE_NAME "my_pipe"
#define SEM_NAME "/my_sem"
#define BUFFER_SIZE 256

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    sem_t *sem;
    pid_t pid;

    // Создать PIPE
    mkfifo(PIPE_NAME, 0666);

    // Создать SEM
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    // Ребёнок
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid == 0) {
        // Запись в PIPE
        fd = open(PIPE_NAME, O_WRONLY);

        sem_wait(sem);
        strcpy(buffer, "Hello, parent process!");
        write(fd, buffer, strlen(buffer) + 1);
        sem_post(sem);

        close(fd);
        return 0;
    } else {
        // Чтение из PIPE
        fd = open(PIPE_NAME, O_RDONLY);

        sem_wait(sem);
        read(fd, buffer, BUFFER_SIZE);
        printf("Received message from child: %s\n", buffer);
        sem_post(sem);

        close(fd);
        wait(NULL);
        return 0;
    }
}
