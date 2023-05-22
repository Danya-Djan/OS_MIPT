#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

void receive_handler(int nsig) {
    // Do nothing, just acknowledge the signal
}

int main(void) {

    pid_t pid, rpid;
    int i,j;
    int num;

    printf("Sender PID: %d\nPlease enter the receiver PID: ", getpid());
    scanf("%d", &rpid);

    printf("Please enter the number to send: ");
    scanf("%d", &num);
    printf("Sending number %d\n", num);

    // Set the signal handler for when the receiver receives a bit
    (void) signal(SIGUSR1, receive_handler);
    (void) signal(SIGUSR2, receive_handler);

    for (int i = 0; i < sizeof(int) * 8; ++i) {
        // If the current bit is 1, send a SIGUSR1 signal to the receiver
        if ((num >> i) & 1) {
            kill(rpid, SIGUSR1);
        } 
        // If the current bit is 0, send a SIGUSR2 signal to the receiver
        else { 
            kill(rpid, SIGUSR2);
        }
        // Wait for a short amount of time to allow the receiver to acknowledge the signal
        for (j = 0; j < 1000000; ++j);
    }

    while(1);
    return 0;
}
