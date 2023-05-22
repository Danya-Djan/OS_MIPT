#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


int a = 0;
void *mythread1(void *variable) {
    pthread_t mythid;
    int num = *((int* )variable);
    mythid = pthread_self();
    printf("Thread %u, my number = %d\n", mythid, num);
    return NULL;
}
void *mythread2 (void *variable) {
    pthread_t mythid;
    int num = *((int*)variable);
    mythid = pthread_self();
    printf("My number = %d, %d\n", mythid, num * num);
    return NULL;
}
int main() {
    pthread_t thid1, thid2, thid3, mythid;
    int result1, result2, result3;

    int variable1 = 1;
    int variable2 = 2;
    int variable3 = 3;

    result1 = pthread_create(&thid1, (pthread_attr_t *)NULL, mythread1, &variable1);
    if (result1 != 0) {
        printf("Error on thread create, return value = %d\n", result1);
        exit (-1);
    }
    printf("First: %u\n", thid1);
    result2 = pthread_create(&thid2, (pthread_attr_t *)NULL, mythread1, &variable2);
    if (result2 != 0) {
        printf("Error on thread create, return value = %d\n", result2);
        exit (-1);
    }
    printf ("Second: %u\n", thid2);
    result3 = pthread_create(&thid3, (pthread_attr_t *)NULL, mythread2, &variable3);
    if (result3 != 0) {
        printf ("Error on thread create, return value = %d\n", result3);
        exit (-1);
    }
    printf ("Third: %u\n", thid3);
    mythid = pthread_self();
    a = a + 1;

    pthread_join(thid1, (void **)NULL);
    pthread_join(thid2, (void **)NULL);
    pthread_join(thid3, (void **)NULL);

    return 0;
}