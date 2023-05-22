#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

#define NUM_CHILD_PROCESSES 10

void my_handler(int nsig){

     static int num_terminated = 0;  // keep track of number of terminated child processes
     int status;
     pid_t pid;

    while (1){

        errno = 0;
        pid = waitpid(-1, &status, WNOHANG);

        if (pid == 0){ // child process not terminated yet
            break;
        }
        else if (pid == -1){ // error
            if (errno == ECHILD){ // no child processes left to wait for
                break;
            }
            else{ // some other error occurred
                printf("Error in waitpid errno = %d\n", errno);
                exit(1);
            }
        }
        else{ // child process terminated
            num_terminated++;
            if ((status & 0xff) == 0) {
                printf("Process %d was exited with status %d\n", pid, status >> 8);
            } else if ((status & 0xff00) == 0){
                printf("Process %d killed by signal %d %s\n", pid, status &0x7f,
                (status & 0x80) ? "with core file" : "without core file");
            }
        }
    }

    if (num_terminated == NUM_CHILD_PROCESSES) { // all child processes have terminated
        printf("All child processes have terminated\n");
        num_terminated = 0;  // reset the counter
    }
}

int main(void){

    pid_t pid;
    int i,j;

    (void) signal(SIGCHLD, my_handler);

    for(i=0; i<NUM_CHILD_PROCESSES; i++){

       if((pid = fork()) < 0){

          printf("Can\'t fork child 1\n");
          exit(1);

       } else if (pid == 0){

          /* Child  */

          for(j=1; j<10000000; )j++;
          exit(200+i);

       }

    }

    while(1);
    return 0;
}
