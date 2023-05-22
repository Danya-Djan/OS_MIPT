#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

int main(void)
{
    int msqid;
    char pathname[]="server.c";
    key_t  key;
    int len, maxlen;

    struct inner_t{
        int pid;
        double number;
    };

    struct mymsgbuf
        long mtype;
        struct inner_t inner;   
    } mybuf;

    key = ftok(pathname, 0);              

    if ((msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL)) < 0){

        if(errno == EEXIST)  
            printf("SERVER: Can\'t start server, msqid already exists\n");
        else
        {
            printf("SERVER: Can\'t create msqid, uknown error\n");            
            perror("SERVER: ");
        }

        exit(-1);
    }

    printf("SERVER: Started\n");

    maxlen = sizeof(struct inner_t);
    
    while(true)
    {
        //try to receive a message from msqid having mtype = -2
        if ((len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, -2, 0)) < 0){
          printf("Can\'t receive message from queue\n");
          exit(-1);
        }

        printf("SERVER: Got: message type=%ld,"
               " info=[pid=%d, number=%lf]\n", 
               mybuf.mtype, mybuf.inner.pid, mybuf.inner.number);

        //Stop receiving messages and destroy the message queue, if pid of inner_t is negative.
        if(mybuf.inner.pid < 0)
        {
            printf("SERVER: Stop working\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(0);
        }
        
        //Stop receiving messages and destroy the message queue, if message-type is 2.
        if(mybuf.mtype == 2)
        {
            printf("SERVER: Stop working\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(0);
        }

        //change the message type and store answer of square of number to message structure, 
        mybuf.mtype = mybuf.inner.pid;
        mybuf.inner.number = mybuf.inner.number * mybuf.inner.number;

        //try to send the message to msqid.
        if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0){
            printf("Can\'t send message to queue\n");
            //msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            //exit(-1);
        }
        else
            printf("SERVER: Answer: message type = %ld,"
                   " info = [pid=%d, number=%lf]\n", 
                   mybuf.mtype, mybuf.inner.pid, mybuf.inner.number);


    }

    return 0;       

