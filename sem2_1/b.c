#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SEM_NAME "/my_sem"

int main()
{
sem_t *sem;
int shmid;
int new = 1;
long i;

sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
if (sem == SEM_FAILED) {
if (errno == EEXIST) {
sem = sem_open(SEM_NAME, 0);
new = 0;
} else {
printf("Can't create semaphore\n");
exit(-1);
}
}

sem_wait(sem);
// critical section
sem_post(sem);

if (new) {
sem_unlink(SEM_NAME);
}

return 0;
}