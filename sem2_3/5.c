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

const int semPlaceOnYacht = 0;
const int semTrap = 1;

//=====================================================================================================

int captainMode(int sem_id, int sem_id2, int countRides, int countSeats);

//=====================================================================================================

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Please, input correct num of arguments");
        return 0;
    }

    int attractionParams[3];
    for (int i = 0; i < 3; ++i) {
        attractionParams[i] = atoi(argv[i + 1]);
        if (attractionParams[i] <= 0) {
            printf("You gave me uncorrect arg number %d\n Please repeat run!\n", attractionParams[i]);
            return 0;
        }
    }

    if (NPASS < NSEAT)
        NSEAT = NPASS;

    int sem_id = semget(IPC_PRIVATE, 1, 0660);  // place on yacht
    int sem_id2 = semget(IPC_PRIVATE, 1, 0660);  // trap
    unsigned short initSemsValue[2] = {0, 1};
    semctl(sem_id, 0, SETVAL, initSemsValue[0]);
    semctl(sem_id2, 0, SETVAL, initSemsValue[1]);

    struct sembuf yachtLanding[] = {{semTrap, -1, 0}, {semPlaceOnYacht, 1, 0}};
    struct sembuf yachtDebarkation[] = {{semPlaceOnYacht, -1, 0}, {semTrap, 1, 0}};

    for (int i = 0; i < NPASS; ++i) {
        int pid = fork();
        if (pid == -1) {
            perror("fork error");
            return 0;
        }

        if (pid) {
            printf("Hi, I'm a [%d] passenger\n", getpid());

            //-----------------------------------------------------------------------------------------------------

            while (1) {
                if (semop(sem_id, yachtLanding, 2) == -1 && (errno == EIDRM || errno == EINVAL)) {
                    printf("Passenger [%d] went into the sunset...\n", getpid());
                    return 0;
                }

                printf("\t\t[%d] set on trap\n", getpid());
                semop(sem_id2, yachtDebarkation, 2);
                printf("\t\t[%d] got off from the yacht to trap\n", getpid());
            }

            //-----------------------------------------------------------------------------------------------------
        }
    }

    // main part

    if (captainMode(sem_id, sem_id2, NRIDE, NSEAT) == -1)
        return -1;

    // wait part

    for (int i = 0; i < NPASS; ++i) {
        wait(NULL);
    }

    semctl(sem_id, 0, IPC_RMID, 0);
    semctl(sem_id2, 0, IPC_RMID, 0);
    printf("\n");

    return 0;
}

//================================================================

int captainMode(int sem_id, int sem_id2, int countRides, int countSeats) {
    struct sembuf yachtLoading[] = {{semPlaceOnYacht, -countSeats, 0}, {semTrap, countSeats, 0}};
    struct sembuf yachtUnloading[] = {{semTrap, -countSeats, 0}, {semPlaceOnYacht, countSeats, 0}};

    printf("The yacht is ready to go!\n");
    for (int i = 0; i < countRides; ++i) {
        printf("===== ride #%d =====\n", i + 1);

        if (semop(sem_id, yachtLoading, 2) == -1) {
            perror("captain error loading");
            return -1;
        }
        printf("\t\tall passengers are on board\n");

        if (semop(sem_id2, yachtUnloading, 2) == -1) {
            perror("captain error unloading");
            return -1;
        }
        printf("\t\tall passengers got off from the yacht\n");
    }
    printf("The yacht is closed for the day!\n");

    return 0;
}