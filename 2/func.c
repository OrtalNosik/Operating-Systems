//ortal nosik 318270378 rotem hadad 207784125
#include "func.h"

Task* Build(char* line){
    //convert each line in the file to a struct called task that has the same fields
    Task* task=malloc(sizeof(Task));
    char* token=strtok(line,",");
    task->id=atoi(token);
    token=strtok(NULL,",");
    task->priority=atoi(token);
    token=strtok(NULL,",");
    task->arrival_time=atoi(token);
    token=strtok(NULL,"\n");
    task->burst_time=atoi(token);
    return task;
}

Task** Table(Task** task_arr, Task* task) {
    //return an array of tasks
    static int place = 0; 
    if (place < MAX) {
        task_arr[place] = task; 
        place++; 
    }
    else {
        printf("Error: Task limit reached!\n");
        free(task); 
    }
    return task_arr;
}

void Display(Task** task_arr, int size) {
    //prints a table that shows the tasks in the file.
    printf("---------------PROCESS TABLE---------------\n");
    printf("ID\t|Priority\t|Arrival Time\t|Burst Time\n");
    printf("-------------------------------------------\n");

    for (int i = 0; i < size; i++) {
        printf("%d\t|%d\t\t|%d\t\t|%d\n", task_arr[i]->id, task_arr[i]->priority, task_arr[i]->arrival_time, task_arr[i]->burst_time);
        printf("-------------------------------------------\n");
    }   
}

void Schedule(enum Algorithm algorithm, Task** task_arr,int size) {
    //The function print the scheduling of an array of tasks
    char* algoName;
    int flag = 0;
    Task** temp_task_arr = malloc(sizeof(Task*) * size);
    memcpy(temp_task_arr, task_arr, sizeof(Task*) * size); // make a copy of the array


    switch (algorithm) {
        case First_Come_First_Served:
            FCFS(temp_task_arr, size);
            algoName="First Come First Served";
            break;
        case Shortest_Job_First:
            SJF(temp_task_arr, size);
            algoName="Shortest Job First";
            break;
        case HRRN:
            HrrN(temp_task_arr, size);
            algoName="HRRN";
            break;
        case Round_Robin:
            flag=1;
            RR(temp_task_arr, size);
            algoName="Round Robin";
            break;
        case Priority_With_Shortest_Job_First:
            MSWSJF(temp_task_arr,size);
            algoName="Priority non preemptive with Shortest Job First";
            break;
        default:
            printf("Error: Invalid algorithm specified!\n");
            break;
    }
    if(flag!=1){
        printf("\nScheduling Tasks- %s Algorithm:\n",algoName);
        for (int i = 0; i < size; i++) {
            printf("<P%d,%d> ", temp_task_arr[i]->id, temp_task_arr[i]->burst_time);
        }
        printf("\n");
        free(temp_task_arr); // free the memory used by the copied array
    }
}

void FCFS(Task** task_arr,int size){
    //implentint the First Come First Served Algorithm
    for (int i=0;i<size;i++){
        for(int j=i+1;j<size;j++){
            if (task_arr[i]->arrival_time > task_arr[j]->arrival_time) {
                Task* tmp = task_arr[i];
                task_arr[i] = task_arr[j];
                task_arr[j] = tmp;
            }
        }
    } 
}



void SJF(Task** task_arr,int size){
    // implentint the Shortest Job First Served Algorithm
    // sort tasks based on arrival time
    FCFS(task_arr,size);
    int time = 0;
    int flag=0;
    for (int i=0;i<size;i++){
        flag=0;
        for (int j=i+1; j<size;j++){
            if (task_arr[j]->burst_time < task_arr[i]->burst_time && task_arr[j]->arrival_time <= time){
                    Task* tmp = task_arr[j];
                    task_arr[j] = task_arr[i];
                    task_arr[i] = tmp;
                    flag = 1;
            }
        }
        if (task_arr[i]->arrival_time <= time){
            time += task_arr[i]->burst_time;
        }
        else if (flag == 0){
            time++;
            i--;
        }
    }

}

void HrrN(Task** task_arr,int size){
    // implentint the Hightst Response Ratio Next Algorithm
    int time = task_arr[0]->arrival_time;
    float x=0;
    int ind = 0;
    Task** tmp=malloc(size*sizeof(Task*));
    float RR[size];
    for(int j=0;j<size;j++){
        for(int i=0;i<size;i++){
            if (task_arr[i]->arrival_time <= time && RR[i] != -1){
                RR[i] = (float)(responseRatio(time - task_arr[i]->arrival_time,task_arr[i]->burst_time));

                if ((float)(RR[i])>=(float)x) { //x keeps the largest rr
                    if((float)(RR[i])==(float)x) {
                        if(task_arr[ind]->arrival_time<=task_arr[i]->arrival_time){
                            continue;
                        }
                    }
                    x=RR[i];
                    ind = i;
                }
            }
        }
        tmp[j] = task_arr[ind];
        x=0;
        RR[ind] = -1;
        time+=task_arr[ind]->burst_time;
    }
    for(int i=0;i<size;i++)
        task_arr[i]=tmp[i];

}


void RR(Task** task_arr,int size){
    printf("\nScheduling Tasks- Round Robin Algorithm:\n");
    int bt[size];
    Task** temp= malloc(size*sizeof(Task*));
    int count_elemnts = 1;
    int count_current = size;
    int time = 0;

    // sort tasks by arrival time
    //FCFS(task_arr,size);
    int i=0;
    int wait[100]; 
    int max_wait=0;
    int flag = 0;
    for(int j=0;j<size;j++){ //initialize
        wait[j]= task_arr[j]->arrival_time;
        bt[j]= task_arr[j]->burst_time;
    }
    while (count_current > 0){
        flag = 0;
        for(int j=0;j<size;j++){ //which p is first in line
            if (wait[j] != -1 && time-wait[j]>=max_wait && task_arr[j]->arrival_time <= time ){
                max_wait=time-wait[j];
                i=j; //save the index
                flag=1;
            }
        }
        if (flag == 0 ){ //no one needs to run
            time++;
            continue;
        }

        if ((task_arr[i]->arrival_time <= time) && (bt[i]>0) && (wait[i] != -1) ){
            if (bt[i] > TIME_QUANTUM){
                bt[i] -= TIME_QUANTUM;
                time+=TIME_QUANTUM;
                wait[i]=time;
                max_wait=0;
                printf("<P%d,%d> ", task_arr[i]->id, TIME_QUANTUM);
            }
            else {
                time+=bt[i];
                printf("<P%d,%d> ", task_arr[i]->id, bt[i]);
                bt[i] = 0;
                count_current--;
                wait[i]=-1;
                max_wait =0;
            }
        }
    }
    printf("\n");
}


float responseRatio(float w,float s){
    return (float)((w+s)/s);
}


void MSWSJF(Task** task_arr,int size){
    FCFS(task_arr,size);
    int ind =0;
    int time = task_arr[0]->arrival_time;
    for(int i=0;i<size;i++){   
        for(int j=i+1;j<size;j++){
            if(task_arr[i]->priority < task_arr[j]->priority && task_arr[j]->arrival_time <= time) { //sort by priority
                Task* tmp = task_arr[i];
                task_arr[i] = task_arr[j];
                task_arr[j] = tmp;
            }
            else if(task_arr[i]->priority == task_arr[j]->priority && task_arr[j]->arrival_time <= time) { //same priority - sort by burst time
                if(task_arr[i]->burst_time > task_arr[j]->burst_time) {
                    Task* tmp = task_arr[i];
                    task_arr[i] = task_arr[j];
                    task_arr[j] = tmp;
                }
            }
        }
        if (task_arr[i]->arrival_time <= time){
            time += task_arr[i]->burst_time;
        }
        else {
            time++;
            i--;
        }

    }

}
