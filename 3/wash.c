//Rotem Hadad 207784125 
//Ortal Nosik 318270378

#include "func.c"

int main(int argc,char *argv[]){
    int numOfMachine= atoi(argv[1]);        //washing stations
    float avg_arrive_time= atoi(argv[2]);   
    float avg_wash_time= atoi(argv[3]);
    float run_time= atoi(argv[4]);
    
    float avg_arrive_time_rate = 1/avg_arrive_time;
    float avg_wash_time_rate = 1/avg_wash_time;
    float run_time_rate = 1/run_time;

    Car* car;
    int car_size;

    //handeling signals
    signal(SIGINT, handleSignal); //c
    signal(SIGTSTP, handleSignal); //z
    signal(SIGQUIT, handleSignal); //"\"
    signal(SIGTERM, handleSignal); //regular
    signal(SIGHUP, handleSignal); // terminal


    //create shared memory
    key_t key = ftok("header.h",SIZE);
    int shm_id = shmget(key,SIZE,IPC_CREAT|0666);
    float* clockTime = (float*)shmat(shm_id,0,0);
    float* waitTime = clockTime+1;
    *clockTime = 0;
    *waitTime = 0;

    //init Semaphores
    sem_t *car_wash_queue = createSemaphore(CAR_WASH_SEM,numOfMachine);
    if(car_wash_queue == SEM_FAILED)
    {
        perror("Error queue semaphore \n");
        exit(1);
    }
    sem_t *time_mutx = createSemaphore(TIME__MUTEX,1);
    if(time_mutx == SEM_FAILED)
    {
        sem_close(car_wash_queue);
        perror("Error queue semaphore \n");
        exit(1);
    }
    sem_t *print_mutx = createSemaphore(PRINT_MUTEX,1);
    if(print_mutx == SEM_FAILED)
    {
        sem_close(car_wash_queue);
        sem_close(time_mutx);
        perror("Error queue semaphore \n");
        exit(1);
    }

    printf("starting Car Wash Simulation\n");
    // seed for rand func
    srand(time(NULL)); 

    float Urandom = nextTime(run_time_rate);
    float Unext ,Uwash;
    for( Unext = nextTime(avg_arrive_time_rate) ; (*clockTime <= Urandom) && (isSignal) ;){
        sem_wait(time_mutx);
        *clockTime = *clockTime + Unext;
        sem_post(time_mutx);
        if(*clockTime <= Urandom)
        {
            car_size++;
            if(fork()==0)
            {
                car = createCar(getpid(),*clockTime);

                sem_wait(print_mutx);
                printf("car : %d , Arrived at time %lf\n",car->pid,car->arriveTime);
                sem_post(print_mutx);
                //entering car wash queue
                sem_wait(car_wash_queue);

                sem_wait(time_mutx);
                car->startTime = *(clockTime);
                *waitTime = *waitTime+ (car->startTime - car->arriveTime);
                sem_post(time_mutx);

                sem_wait(print_mutx);
                printf("car : %d , in the washing machine! ,Time %lf\n",car->pid,car->startTime);
                sem_post(print_mutx);

                Uwash = nextTime(avg_wash_time_rate);
                car->endTime=car->startTime + Uwash;
                sleep(Uwash);

                sem_post(car_wash_queue);
                //

                //access to time
                sem_wait(time_mutx);
                if(car->endTime > *(clockTime))
                    *(clockTime) = car->endTime;
                sem_post(time_mutx);
                
                //access to print
                sem_wait(print_mutx);
                printf("car : %d , leave the washing machine! , Time %f\n",car->pid,car->endTime);
                sem_post(print_mutx);

                if(car)
                    free(car);

                exit(1);
            }
            else{
                //main(father) awaits for next car to arrive
                Unext = nextTime(avg_arrive_time_rate);
                sleep(Unext);           
            }
        }
    }
    //wait for all the childs to finish
    while (wait(NULL)>0) ;            
    printf("Time taken = %lf seconds, car washed = %d, Avg waiting time = %lf\n",*clockTime,car_size,*waitTime/car_size);
    

    //free shared memeory
    sem_close(car_wash_queue);
    sem_close(time_mutx);
    sem_close(print_mutx);
    shmctl(shm_id,IPC_RMID,0);
    shmdt(clockTime);
    shmdt(waitTime);

    return 0;
}

