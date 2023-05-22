#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void)
{
    int msqid;
    char pathname[]="server.c";

    key_t key;
    int len;

    struct inner_t{
        int pid;
        double number;
    };

    struct mymsgbuf
    {
        long mtype;
        struct inner_t inner;
    } mybuf;

    key = ftok(pathname, 0);

    if ((msqid = msgget(key, 0))<0){
       printf("Can\'t find msqid, probably server not started\n");
       exit(-1);
    }

    mybuf.mtype = 2;
    mybuf.inner.pid = getpid();
    mybuf.inner.number = 0;

    len = sizeof(struct inner_t);

    //Try to send the message to msqid.
    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0){
        printf("Can\'t send message to queue\n");
    }
    else
        printf("CLIENT: Send: message type = %ld,"
               " info = [pid = %d, number = %lf]\n", 
               mybuf.mtype, mybuf.inner.pid, mybuf.inner.number);

    //Max length for message received. In this case, it's length of inner_t
    int maxlen = sizeof(struct inner_t);
    
    //Get the type of message to be received, which is PID of client process
    int type = mybuf.inner.pid;

    //Receive status of server after sending a server-kill message, no wait.
    if((len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, type, IPC_NOWAIT)) < 0){
      printf("KILLER: Server was killed successfully\n"); //Server stopped
    }
    else
    {
        printf("KILLER: Failure occurred, server still works\n"); //Server still running
        exit(-1);
    }

    return 0;    
}
