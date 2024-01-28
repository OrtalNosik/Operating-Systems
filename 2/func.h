//ortal nosik 318270378 rotem hadad 207784125
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 10
#define TIME_QUANTUM 2

typedef struct{
    int id;
    int priority;
    int arrival_time;
    int burst_time;
} Task;

enum Algorithm{
    First_Come_First_Served=1,
    Shortest_Job_First=2,
    HRRN=3,
    Round_Robin=4,
    Priority_With_Shortest_Job_First=5
};

Task* Build(char*);
Task** Table(Task**,Task*);
void Display(Task**,int);
void Schedule(enum Algorithm,Task**,int);
void FCFS(Task**,int);
void SJF(Task**,int);
void HrrN(Task**,int);
void RR(Task**,int);
void MSWSJF(Task**,int);

float responseRatio(float w,float s);