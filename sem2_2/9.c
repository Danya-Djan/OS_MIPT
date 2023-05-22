#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

int main() {
  // Create a pipe for interprocess communication
  int fd[2];
  if (pipe(fd) < 0) {
    printf("Can't open pipe\n");
    exit(-1);
  }

  // Create a semaphore set with one semaphore
  char pathname[] = "9.c";
  key_t sem_key;
  int semid;
  if ((sem_key = ftok(pathname, 0)) < 0) {
    printf("Can't generate semaphore key\n");
    exit(-1);
  }

  // Try to create the semaphore set, or get the existing one if it already exists
  if ((semid = semget(sem_key, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0) {
    if (errno != EEXIST) {
      printf("Can't create semaphore set\n");
      exit(-1);
    } else {
      if ((semid = semget(sem_key, 1, 0)) < 0) {
        printf("Can't find semaphore\n");
        exit(-1);
      }
    }
  }

  // Set up the semaphore buffer
  struct sembuf my_buf;
  my_buf.sem_num = 0;
  my_buf.sem_flg = 0;

  // Fork a child process
  int result = fork();
  if (result < 0) {
    printf("Can't fork child\n");
    exit(-1);
  } else if (result > 0) {
    // Parent process
    for (int i = 0; i < 500; i++) {    
      // Write to the pipe
      size_t size = write(fd[1], "Hello, child!!", 14);
      if (size != 14) {
        printf("Parent: Can't write all string to pipe\n");
        exit(-1);
      }

      // Increase the semaphore value by 2
      my_buf.sem_op = 2;
      if (semop(semid, &my_buf, 1) < 0) {
        printf("Parent: Can't wait for condition\n");
        exit(-1);
      }

      // Wait for the semaphore value to be 0
      my_buf.sem_op = 0;
      if (semop(semid, &my_buf, 1) < 0) {
        printf("Parent: Can't wait for condition\n");
        exit(-1);
      }

      // Read from the pipe
      size = read(fd[0], resstring, 14);
      if (size < 0) {
        printf("Parent: Can't read string from pipe\n");
        exit(-1);
      }
      printf("Parent: received \"%s\"\n", resstring);
    }
  } else {
    // Child process
    for (int i = 0; i < 500; i++) {
      // Decrease the semaphore value by 1
      my_buf.sem_op = -1;
      if (semop(semid, &my_buf, 1) < 0) {
        printf("Child: Can't wait for condition\n");
        exit(-1);
      }

      // Read from the pipe
      size_t size = read(fd[0], resstring, 14);
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
