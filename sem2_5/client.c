#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void){
    int msqid; // Message queue ID number, unique to each message queue.
    char pathname[] = "server.c"; 
    key_t key; // Unique value calculated using ftok().
    int len; 

    // Struct inner_t holds process ID and a double precision number.
    struct inner_t{
        int pid;
        double number;
    };

    struct mymsgbuf
    {
        long mtype; 
        struct inner_t inner; 
    } mybuf;

    // Use of the ftok() function generates a unique identifier for the message queue.
    key = ftok(pathname, 0);

    // If msgget is called with IPC_CREAT and the queue does not exist, it creates the queue.
    if((msqid = msgget(key, 0)) < 0){ 
       printf("Can\'t find msqid, probably server not started\n");
       exit(-1);
    }

    // Store data in mybuf before sending.
    mybuf.mtype = 1; 
    mybuf.inner.pid = getpid();

    printf("CLIENT: Enter the number\n");
    scanf("%lf", &mybuf.inner.number);

    // If user enters a number greater than 10, update pid to -1.
    if(mybuf.inner.number > 100)
        mybuf.inner.pid = -1;

    len = sizeof(struct inner_t);

    // Attempt to send message via message queue.
    if(msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0){
        printf("Can\'t send message to queue\n");
        //msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        //exit(-1);
    }
    else
        printf("CLIENT: Send: message type = %ld, info = [pid = %d, number = %lf]\n", 
               mybuf.mtype, mybuf.inner.pid, mybuf.inner.number);

    int maxlen = sizeof(struct inner_t);
    
    int type = mybuf.inner.pid;

    // Check if any message received back from server process via message queue.
    if((len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, type, 0)) < 0){
        printf("CLIENT: Can\'t receive message from queue\n");
        exit(-1);
    }
    else
        printf("CLIENT: Got: message type = %ld,info = [pid = %d, number = %lf]\n", 
               mybuf.mtype, mybuf.inner.pid, mybuf.inner.number);

    return 0;    
}
