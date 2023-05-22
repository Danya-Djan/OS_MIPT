#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int msqid;
    char pathname[]="cc.c";
    key_t  key;
    int len, maxlen;

    struct inner_t{
        double c;
        short b;
        char a;
    };

    struct mymsgbuf
    {
        long mtype;
        struct inner_t inner;
    } mybuf;
    
    key = ftok(pathname, 0);
    
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
       printf("Can\'t get msqid\n");
       exit(-1);
    }
    
    /* Send information */
    mybuf.mtype = 2;
    mybuf.inner.a = 4;
    mybuf.inner.b = 5;
    mybuf.inner.c = 6;
       
    len = sizeof(struct inner_t);

    if (msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0) < 0){
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }

    /* Receive information */

    maxlen = sizeof(struct inner_t);
    
    if (( len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 1, 0)) < 0){
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }

    printf("[dd.c] message type = %ld, info = [%lf, %hd, %hhd]\n", 
    mybuf.mtype, mybuf.inner.c, mybuf.inner.b, mybuf.inner.a);

    return 0;       
}
