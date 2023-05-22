#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

int main() {
    int fd[2], result;
    size_t size;
    char resstring[14];

    if (pipe(fd) < 0) {
        perror("Can't open first pipe");
        exit(EXIT_FAILURE);
    }

    char pathname[] = "task.c";
    key_t sem_key;
    int semid;

    if ((sem_key = ftok(pathname, 0)) < 0) {
        perror("Can't generate semaphore key");
        exit(EXIT_FAILURE);
    }

    if ((semid = semget(sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        if (errno != EEXIST) {
            perror("Can't create semaphore set");
            exit(EXIT_FAILURE);
        } else {
            if ((semid = semget(sem_key, 1, 0)) < 0) {
                perror("Can't find semaphore");
                exit(EXIT_FAILURE);
            }
        }
    }

    struct sembuf my_buf;
    my_buf.sem_num = 0;
    my_buf.sem_flg = 0;

    result = fork();

    if (result < 0) {
        perror("Can't fork child");
        exit(EXIT_FAILURE);
    } else if (result > 0) {
        for (int i = 0; i < 500; i++) {
            // Write to pipe
            size = write(fd[1], "Hello, child!!", 14);
            if (size != 14) {
                perror("Parent: Can't write all string to pipe");
                exit(EXIT_FAILURE);
            }

            // a(2)
            my_buf.sem_op = 2;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("Parent: Can't wait for condition");
                exit(EXIT_FAILURE);
            }

            // z()
            my_buf.sem_op = 0;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("Parent: Can't wait for condition");
                exit(EXIT_FAILURE);
            }

            // Read from pipe
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                perror("Parent: Can't read string from pipe");
                exit(EXIT_FAILURE);
            }
            printf("Parent: received \"%s\"\n", resstring);
        }
    } else {
        for (int i = 0; i < 500; i++) {
            // d(1)
            my_buf.sem_op = -1;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("Child: Can't wait for condition");
                exit(EXIT_FAILURE);
            }

            // Read from pipe
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                perror("Child: Can't read string from pipe");
                exit(EXIT_FAILURE);
            }
            printf("Child: received \"%s\"\n", resstring);

            // Write to pipe
            size = write(fd[1], "Hello, parent!", 14);
            if (size != 14) {
                perror("Child: Can't write all string to pipe");
                exit(EXIT_FAILURE);
            }

            // d(1)
            my_buf.sem_op = -1;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("semop");
                exit(EXIT_FAILURE);
            }
        }
    }
}