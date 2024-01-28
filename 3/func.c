//Rotem Hadad 207784125 
//Ortal Nosik 318270378

#include "header.h"

//given nexttime function
float nextTime(float rateParameter)
{
    return -logf(1.0f - (float)rand() / ((float)RAND_MAX + 1)) / rateParameter;
}

sem_t* createSemaphore(char* sem_name,int size)
{
    sem_unlink(sem_name);
    return sem_open(sem_name, O_CREAT | O_EXCL ,0660,size);
}

Car* createCar(int id,float time)
{
    Car* c = (Car*)malloc(sizeof(Car));
    c->pid = id;
    c->startTime = 0;
    c->arriveTime = time;
    c->endTime = 0;
    return c;
}

void handleSignal(){
    printf(" ");
    isSignal = 0;
}