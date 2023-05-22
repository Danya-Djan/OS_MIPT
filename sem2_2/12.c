#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define SEM_KEY_FILE "12.c"
#define SEM_KEY_ID 0
#define SEM_PERMISSIONS 0666

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define PIPE_BUFFER_SIZE_CHILD 15
#define PIPE_BUFFER_SIZE_PARENT 16
#define INTERATION_COUNT 20

int generate_sem();
void parent(int fd_read, int fd_write, int semid);
void child(int fd_read, int fd_write, int semid); 

int main() {
    int fd[2];
    if (pipe(fd) < 0) {
        printf("Can't open first pipe\n");
        exit(EXIT_FAILURE);
    }
    int semid = generate_sem();
    int result = fork();
    if (!result) { parent(fd[0], fd[1], semid); }
    else { child(fd[0], fd[1], semid); }

    return 0;
}

void parent(int fd_read, int fd_write, int semid) {
    struct sembuf sem_buf = {0, 0, 0};
    for (int i = 0; i < INTERATION_COUNT; i++) {

        if (write(fd_write, "Hello, child!\0", PIPE_BUFFER_SIZE_CHILD) != PIPE_BUFFER_SIZE_CHILD) {
            exit(EXIT_FAILURE);
        }

        sem_buf.sem_op = 2;
        if (semop(semid, &sem_buf, 1) < 0) {
            exit(EXIT_FAILURE);
        }

        sem_buf.sem_op = 0;
        if (semop(semid, &sem_buf, 1) < 0) {
            exit(EXIT_FAILURE);
        }

        char resstring[PIPE_BUFFER_SIZE_PARENT];
        ssize_t size = read(fd_read, resstring, PIPE_BUFFER_SIZE_PARENT);
        if (size < 0) {
            exit(EXIT_FAILURE);
        }
        printf("parent: received \"%s\"\n", resstring);
    }
}

void child(int fd_read, int fd_write, int semid) {
    struct sembuf sem_buf = {0, -1, 0};
    for (int i = 0; i < INTERATION_COUNT; i++) {

        if (semop(semid, &sem_buf, 1) < 0) {
            exit(EXIT_FAILURE);
        }

        char resstring[PIPE_BUFFER_SIZE_PARENT];
        ssize_t size = read(fd_read, resstring, PIPE_BUFFER_SIZE_CHILD);
        if (size < 0) {
            exit(EXIT_FAILURE);
        }
        printf("child: received \"%s\"\n", resstring);

        
        if (write(fd_write, "Hello, parent!\0", PIPE_BUFFER_SIZE_PARENT) != PIPE_BUFFER_SIZE_PARENT) {
            exit(EXIT_FAILURE);
        }

        sem_buf.sem_op = -1;
        if (semop(semid, &sem_buf, 1) < 0) {
            exit(EXIT_FAILURE);
        }
    }
}

int generate_sem() {
    key_t sem_key = ftok(SEM_KEY_FILE, SEM_KEY_ID);
    if (sem_key < 0) {
        exit(EXIT_FAILURE);
    }
    
    int semid = semget(sem_key, 1, SEM_PERMISSIONS | IPC_CREAT | IPC_EXCL);
    if (semid < 0) {
        if (errno != EEXIST) {
            exit(EXIT_FAILURE);
        } else {
            semid = semget(sem_key, 1, 0);
            if (semid< 0) {
                exit(EXIT_FAILURE);
            }
        }
    }

    return semid;
}