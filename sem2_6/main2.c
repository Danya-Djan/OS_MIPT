#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

void *mythread(void *arg) {
    int mynum = *(int *)arg;
    printf("Thread %d\n", mynum);
    return NULL;
}

int main() {
    pthread_t thid[NUM_THREADS];
    int num[NUM_THREADS];
    int result;

    for (int i = 0; i < NUM_THREADS; ++i) {
        num[i] = i;
        result = pthread_create(&thid[i], NULL, mythread, &num[i]);
        if (result != 0) {
            fprintf(stderr, "Error: pthread_create failed with error code %d\n", result);
            exit(EXIT_FAILURE);
        }
        pthread_join(thid[i], NULL);
    }

    return 0;
}
