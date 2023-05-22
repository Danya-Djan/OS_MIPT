#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int a = 0;

void* mythread1(void* arg) {
    int num = *(int*)arg;
    pthread_t mythid = pthread_self();
    printf("Thread %lu, my number = %d\n", mythid, num);
    return NULL;
}

void* mythread2(void* arg) {
    int num = *(int*)arg;
    pthread_t mythid = pthread_self();
    printf("Thread %lu, my number = %d\n", mythid, num * num);
    return NULL;
}

int main() {
    int result;
    pthread_t thid1, thid2, thid3;

    int variable1 = 1;
    int variable2 = 2;
    int variable3 = 3;

    result = pthread_create(&thid1, NULL, mythread1, &variable1);
    if (result != 0) {
        printf("Error on thread create, return value = %d\n", result);
        exit(-1);
    }
    printf("First thread created with ID %lu\n", thid1);

    result = pthread_create(&thid2, NULL, mythread1, &variable2);
    if (result != 0) {
        printf("Error on thread create, return value = %d\n", result);
        exit(-1);
    }
    printf("Second thread created with ID %lu\n", thid2);

    result = pthread_create(&thid3, NULL, mythread2, &variable3);
    if (result != 0) {
        printf("Error on thread create, return value = %d\n", result);
        exit(-1);
    }
    printf("Third thread created with ID %lu\n", thid3);

    pthread_join(thid1, NULL);
    pthread_join(thid2, NULL);
    pthread_join(thid3, NULL);

    pthread_t mythid = pthread_self();
    a = a + 1;
    printf("Main thread %lu exiting\n", mythid);

    return 0;
}
