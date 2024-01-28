#include "func.c"

int main(int argc,char **argv)
{
    printf("---Starting Custom Shell---\n");
    printf("---------------------------\n");
    int childPid;
    char * cmdLine;
    parseInfo *info;
    while(1){
        printPath();
        cmdLine = readline(" >> ");
        info = parse(cmdLine);

        childPid=fork();
        if (childPid==0)
        {
            /*child code*/
            executeCommand(info);//calls execvp
        }
        else{
            /* parent code */
            waitpid(childPid,NULL,0);
        }
    }
}