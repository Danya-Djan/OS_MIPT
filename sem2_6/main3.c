#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 10

int a = 0;

void *mythread(void *dummy) {
    pthread_t mythid;
    int mynum = ((int *)dummy)[0];
    while(!(mynum == a));
    a = a + 1;
    printf("Thread - %d", mynum, a);
    return NULL;
}

int main(){
   
    pthread_t thid, mythid; 
    int       result;
    int num[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; ++i)
        num[i] = i;

    for(int i = 0; i < NUM_THREADS; ++i){
        result = pthread_create( &thid, (pthread_attr_t *)NULL, mythread, (void *)(num + i));
    
        if(result != 0){
            printf ("Error: %d\n", result);
            exit(-1);
        }   
    }
    
    mythid = pthread_self();
    pthread_join(thid, (void **)NULL);
   
   return 0;
}
