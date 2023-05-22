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
const int semTrap = 1;
const int semBanEnter = 2;

//=====================================================================================================

int captainMode(int sem_id, int countRides, int countSeats);  // ))))

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

    int sem_id = semget(IPC_PRIVATE, 3, 0660);  // place on yacht; trap; ban of enter
    unsigned short initSemsValue[3] = {0, 0, 0};
    semctl(sem_id, 0, SETALL, initSemsValue);

    struct sembuf landing[] = {{semBanEnter, 0, 0}, {semTrap, -1, 0}, {semPlaceOnYacht, -1, 0}};
    struct sembuf debarkation[] = {{semTrap, -1, 0}, {semPlaceOnYacht, 1, 0}};
    struct sembuf freeTrap[] = {{semTrap, 1, 0}};

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
                if ((semop(sem_id, landing, 3) == -1) && ((errno == EIDRM) || (errno == EINVAL))) {
                    printf("Passenger [%d] went into the sunset...\n", getpid());
                    return 0;
                }

                printf("\t\t[%d] set on trap\n", getpid());
                semop(sem_id, freeTrap, 1);
                printf("\t\t[%d] got off from trap to yacht\n", getpid());
                semop(sem_id, debarkation, 2);
                printf("\t\t[%d] got off from the yacht to trap\n", getpid());
                semop(sem_id, freeTrap, 1);
                printf("\t\t[%d] got off from the trap to Earth. Finally\n", getpid());
            }

            //-----------------------------------------------------------------------------------------------------
        }
    }

    // main part

    if (captainMode(sem_id, NRIDE, NSEAT) == -1)
        return -1;

    // wait part

        for (int i = 0; i < NPASS; ++i) {
        wait(NULL);
    }

    semctl(sem_id, 0, IPC_RMID, 0);
    printf("\n");

    return 0;
}

//=====================================================================================================

int captainMode(int sem_id, int countRides, int countSeats) {
    printf("Hi, I'm the captain! Let's start our attraction!\n");

    struct sembuf yachtLanding[] = {{semBanEnter, countSeats, 0}, {semPlaceOnYacht, countSeats, 0}, {semBanEnter, -countSeats, 0}};
    struct sembuf yachtDebarkation[] = {{semPlaceOnYacht, -countSeats, 0}};

    for (int i = 0; i < countRides; ++i) {
        printf("Ride [%d] of [%d]\n", i + 1, countRides);

        for (int j = 0; j < countSeats; ++j) {
            semop(sem_id, &yachtLanding[j], 2);
        }

        printf("\t The yacht is full! Let's sail!!!\n");

        usleep(500);

        for (int j = 0; j < countSeats; ++j) {
            semop(sem_id, &yachtDebarkation[0], 1);
        }

        printf("\t The yacht is empty! Let's load new passengers!!!\n");
    }

    printf("The attraction is over!\n");
    return 0;
}

