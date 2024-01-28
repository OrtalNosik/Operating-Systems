//ortalnosik 318270378 rotemhadad 207784125
#include "func.c"

void main(int argc,char *argv[]){
    Task** task_arr=malloc(MAX*sizeof(Task*));
    FILE* file=fopen(argv[1],"r");
    if (!file){
        printf("Error openning file %s\n",argv[1]);
    }
    char line[MAX];
    int size=0;
    while(fgets(line,sizeof(line),file)!=NULL){
        task_arr=Table(task_arr,Build(line));
        size++;
    }
    Display(task_arr,size);

    Schedule(First_Come_First_Served,task_arr,size);
    Schedule(Shortest_Job_First,task_arr,size);
    Schedule(HRRN,task_arr,size);
    Schedule(Round_Robin,task_arr,size);
    Schedule(Priority_With_Shortest_Job_First,task_arr,size);
    free(task_arr);
}


