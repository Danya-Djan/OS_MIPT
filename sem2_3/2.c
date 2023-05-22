#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//=====================================================================================================

#define NPASS attractionParams[0]  // num of passengers
#define NRIDE attractionParams[1]  // limit of rides
#define NSEAT attractionParams[2]  // num of seats

const int wideTrap = 1;
const int semPlaceOnYacht = 0;
const int semBanOfDebarkation = 1;
const int semTrap = 2;
const int semBanEnter = 3;

//=====================================================================================================

void passengerMode(int sem_id);
int captainMode (int sem_id, int countRides, int countSeats);  // ))))

//=====================================================================================================

int main (int argc, char *argv[])
{
    if (argc != 4) {
        printf ("Please, input correct num of arguments");
        return 0;
    }

    int attractionParams[3];
    for (int i = 0; i < 3; ++i) {
        attractionParams[i] = atoi (argv[i + 1]);
        if (attractionParams[i] <= 0) {
            printf ("You gave me uncorrect arg number %d\n Please repeat run!\n", attractionParams[i]);
            return 0;
        }
    }

    if (NPASS < NSEAT)
        NSEAT = NPASS;

    int sem_id = semget (IPC_PRIVATE, 4, 0660);  // place on yacht; ban of debarcation; trap; ban of enter
    if (sem_id == -1) {
        perror("Semaphore creation error");
        return -1;
    }
    unsigned short initSemsValue[4] = {0, 0, 0, 0};
    if (semctl (sem_id, 0, SETALL, initSemsValue) == -1) {
        perror("Semaphore initialization error");
        return -1;
    }

    struct sembuf landing[] = {{semBanEnter, 0, 0}, {semTrap, -1, 0}, {semPlaceOnYacht, -1, 0}, {semBanOfDebarkation, 1, 0}};
    struct sembuf debarkation[] = {{semBanOfDebarkation, 0, 0}, {semTrap, -1, 0}, {semPlaceOnYacht, 1, 0}};
    struct sembuf freeTrap[] = {{semTrap, 1, 0}};

    for (int i = 0; i < NPASS; ++i) {
        pid_t pid = fork ();
        if (pid == -1) {
            perror ("fork error");
            return 0;
        }

        if (pid == 0) { // child process
            passengerMode(sem_id);
            exit(0);
        }
    }

    // main part

    if (captainMode (sem_id, NRIDE, NSEAT) == -1)
        return -1;

    // wait part

    for (int i = 0; i < NPASS + 1; ++i)
        wait (NULL);

    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Semaphore removal error");
        return -1;
    }

    return 0;
}

//=====================================================================================================

void passengerMode(int sem_id) {
    printf ("Hi, I'm a [%d] passenger\n", getpid ());
    semop (sem_id, freeTrap, 1);
    printf ("\t\t[%d] got off from trap to yacht\n", getpid ());
    semop (sem_id, debarkation, 3);
    printf ("\t\t[%d] got off from the yacht to trap\n", getpid ());
    semop (sem_id, freeTrap, 1);
    printf ("\t\t[%d] got off from the trap to Earth. Finally\n", getpid ());
}

            //-----------------------------------------------------------------------------------------------------
return 0;

