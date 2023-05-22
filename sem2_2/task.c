#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

int main() {
    int fd[2], result;
    size_t size;
    char resstring[14];
    if (pipe(fd) < 0) {
        printf("Can\'t open first pipe\n");
        exit(-1);
    }

    char pathname[] = "task.c";
    key_t sem_key;
    int semid;

    if ((sem_key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate semaphore key\n");
        exit(-1);
    }

    if ((semid = semget(sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
        if (errno != EEXIST) {
            printf("Can\'t create semaphore set\n");
            exit(-1);
        } else {
            if ((semid = semget(sem_key, 1, 0)) < 0) {
                printf("Can\'t find semaphore\n");
                exit(-1);
            }
        }
    }

    struct sembuf my_buf;
    my_buf.sem_num = 0;
    my_buf.sem_flg = 0;

    result = fork();

    if (result < 0) {
        printf("Can\'t fork child\n");
    } else if (result > 0) {
        for (int i = 0; i < 500; i++) {    
            // write
            size = write(fd[1], "Hello, child!!", 14);
            if (size != 14) {
                printf("parent: Can\'t write all string to pipe\n");
                exit(-1);
            }

            // a(2)
            my_buf.sem_op = 2;
            if (semop(semid, &my_buf, 1) < 0) {
                printf("parent: Can\'t wait for condition\n");
                exit(-1);
            }
            
            // z()
            my_buf.sem_op = 0;
            if (semop(semid, &my_buf, 1) < 0) {
                printf("parent: Can\'t wait for condition\n");
                exit(-1);
            }

            // read
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                printf("parent: Can\'t read string from pipe\n");
                exit(-1);
            }
            printf("parent: recieved \"%s\"\n", resstring);
        }
    } else {
        for (int i = 0; i < 500; i++) {
            // d(1)
            my_buf.sem_op = -1;
            if (semop(semid, &my_buf, 1) < 0) {
                printf("child: Can\'t wait for condition\n");
                exit(-1);
            }

            // read
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                printf("child: Can\'t read string from pipe\n");
                exit(-1);
            }
            printf("child: recieved \"%s\"\n", resstring);

            // write
            size = write(fd[1], "Hello, parent!", 14);
            if (size != 14) {
                printf("child: Can\'t write all string to pipe\n");
                exit(-1);
            }

            // d(1)
            my_buf.sem_op = -1;
            if (semop(semid, &my_buf, 1) < 0) {
                printf("child: Can\'t wait for condition\n");
                exit(-1);
            }
        }
    }
}
