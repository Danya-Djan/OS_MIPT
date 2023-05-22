#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>

#define SEMAPHORE_KEY_PATH "task.c"
#define SEMAPHORE_KEY_ID 0
#define SEMAPHORE_PERMISSIONS 0666

int main() {
    int fd[2], result;
    size_t size;
    char resstring[14];
    if (pipe(fd) < 0) {
        perror("Can't open first pipe");
        exit(EXIT_FAILURE);
    }

    key_t sem_key;
    int semid;

    if ((sem_key = ftok(SEMAPHORE_KEY_PATH, SEMAPHORE_KEY_ID)) < 0) {
        perror("Can't generate semaphore key");
        exit(EXIT_FAILURE);
    }

    if ((semid = semget(sem_key, 1, SEMAPHORE_PERMISSIONS | IPC_CREAT | IPC_EXCL)) < 0) {
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
            // write
            size = write(fd[1], "Hello, child!!", 14);
            if (size != 14) {
                perror("parent: Can't write all string to pipe");
                exit(EXIT_FAILURE);
            }

            // a(2)
            my_buf.sem_op = 2;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("parent: Can't wait for condition");
                exit(EXIT_FAILURE);
            }
            
            // z()
            my_buf.sem_op = 0;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("parent: Can't wait for condition");
                exit(EXIT_FAILURE);
            }

            // read
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                perror("parent: Can't read string from pipe");
                exit(EXIT_FAILURE);
            }
            printf("parent: received \"%s\"\n", resstring);
        }
    } else {
        for (int i = 0; i < 500; i++) {
            // d(1)
            my_buf.sem_op = -1;
            if (semop(semid, &my_buf, 1) < 0) {
                perror("child: Can't wait for condition");
                exit(EXIT_FAILURE);
            }

            // read
            size = read(fd[0], resstring, 14);
            if (size < 0) {
                perror("child: Can't read string from pipe");
                exit(EXIT_FAILURE);
            }
            printf("child: received \"%s\"\n", resstring);

            // write
            size = write(fd[1], "Hello, parent!", 14);
            if (size != 14) {
                perror("child: Can't write all string to pipe\n");
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
            