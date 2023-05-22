#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

int received_number = 0; // variable to store the received number
int bit_count = 0; // variable to keep track of the number of bits received

void handle_bit_received(int signal_number) {
    if (signal_number == SIGUSR1) {
        received_number |= (1 << bit_count);
    }
    ++bit_count;
}

int main(void){

    pid_t sender_pid, receiver_pid;

    printf("Receiver PID: %d\nPlease enter the sender PID: ", getpid());
    scanf("%d", &sender_pid);

    // Register the signal handlers
    (void) signal(SIGUSR1, handle_bit_received);
    (void) signal(SIGUSR2, handle_bit_received);

    // Wait until all bits of the number have been received
    while (bit_count < sizeof(int) * 8) {
        // Do nothing
    }

    printf("Received number: %d\n", received_number);
    kill(sender_pid, SIGUSR1);

    while(1);

    return 0;
}
