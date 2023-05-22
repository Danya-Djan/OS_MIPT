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

//=====================================================================================================

int captainMode(int sem_id, int countRides, int countSeats);  // ))))

//=====================================================================================================

struct sembuf landing[] = {{semBanOfDebarkation, 0, 0}, {semPlaceOnYacht, -NSEAT, 0}};
struct sembuf debarkation[] = {{semPlaceOnYacht, NSEAT, 0}, {semBanOfDebarkation, NSEAT, 0}};

int main(int argc, char* argv[])
{
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

    int sem_id = semget(IPC_PRIVATE, 2, 0660);  // place on yacht; ban of debarcation
    unsigned short initSemsValue[2] = {0, 0};
    semctl(sem_id, 0, SETALL, initSemsValue);

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
                if ((semop(sem_id, landing, 2) == -1) && ((errno == EIDRM) || (errno == EINVAL))) {
                    printf("Passenger [%d] went into the sunset...\n", getpid());
                    return 0;
                }

                printf("\t\t[%d] set on trap\n", getpid());
                semop(sem_id, debarkation, 2);
                printf("\t\t[%d] got off from the yacht to trap\n", getpid());
            }

            //-----------------------------------------------------------------------------------------------------
        }
    }

    // main part

    if (captainMode(sem_id, NRIDE, NSEAT) == -1)
        return -1;

    // wait part

    for (int i = 0; i < NPASS + 1; ++i)
        wait(NULL);

    semctl(sem_id, 0, IPC_RMID, 0);

    return 0;
}

//=====================================================================================================

int captainMode(int sem_id, int countRides, int countSeats)
{
    int res = 0;

    for (int i = 0; i < countRides; ++i) {
        printf("Captain started the %d ride\n", i + 1);
        for (int j = 0; j < countSeats; ++j) {
            semop(sem_id, &landing[wideTrap], 1);
            printf("Passenger [%d] took a seat [%d]\n", getpid(), j + 1);
        }
        printf("All passengers are on board. The yacht is starting...\n");
        sleep(2);
        semop(sem_id, &debarkation[wideTrap], 1);
        printf("All passengers have left the yacht\n");
    }

    return res;
}

       
