//Rotem Hadad 207784125 
//Ortal Nosik 318270378

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>

//define shm size
#define SIZE 10

//define semphores name
#define CAR_WASH_SEM "CW"
#define TIME__MUTEX "TM"
#define PRINT_MUTEX "PM"

static int isSignal = 1;

//car as an thread
typedef struct Car
{
    pid_t pid;
    float arriveTime;
    float startTime;
    float endTime;

}Car;

float nextTime(float rateParameter);
sem_t* getSemaphore(char* sem_name,int size);
Car* getCar(int id,float time);
void handleSignal();

