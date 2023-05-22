#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

int main() {
    int fd[2], result;
    size_t size;
    char resstring[14];

    // create a pipe
    if (pipe(fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // create a semaphore
    char pathname[] = "10.c";
    key_t sem_key;
    int semid;

    if ((sem_key = ftok(pathname, 0)) < 0) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    semid = semget(sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (semid < 0) {
        if (errno == EEXIST) {
            semid = semget(sem_key, 1, 0);
            if (semid < 0) {
                perror("semget");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("semget");
            exit(EXIT_FAILURE);
        }
    }

    // initialize semaphore operations
    struct sembuf sem_buf;
    sem_buf.sem_num = 0;
    sem_buf.sem_flg = 0;

    result = fork();

    if (result < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (result > 0) {
        // parent process
        for (int i = 0; i < 500; i++) {    
            // write to pipe
            size = write(fd[1], "Hello, child!", 14);
            if (size != 14) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            // signal the semaphore
            sem_buf.sem_op = 2;
            if (semop(semid, &sem_buf, 1) < 0) {
                perror("semop");
                exit(EXIT_FAILURE);
            }
            
            // wait for the semaphore
            sem_buf.sem_op = 0;
            if (semop(semid, &sem_buf, 1) < 0) {
                perror("semop");
                exit(EXIT_FAILURE);
            }

            // read from pipe
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            printf("parent: received \"%s\"\n", resstring);
        }
    } else {
        // child process
        for (int i = 0; i < 500; i++) {
            // wait for the semaphore
            sem_buf.sem_op = -1;
            if (semop(semid, &sem_buf, 1) < 0) {
                perror("semop");
                exit(EXIT_FAILURE);
            }

            // read from pipe
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            printf("child: received \"%s\"\n", resstring);

            // write to pipe
            size = write(fd[1], "Hello, parent!", 14);
            if (size != 14) {
                perror("write");
                exit(EXIT_FAILURE);
            }

            // signal the semaphore
            sem_buf.sem_op = 1;
            if (semop(semid, &sem_buf, 1) < 0) {
                perror("semop");
                exit(EXIT_FAILURE);
            }
        }
    }
}
