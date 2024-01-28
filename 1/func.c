#include "headers.h"

parseInfo* parse(char* line){ //parse user command - check logic and valid
    int num_of_tokens = checkNumOfTokens(line);
    if (strlen(line) == 0){
        return NULL;
    }
    char* token = strtok(line, " |");
    if (strcmp(token, "exit") == 0){ //check exit separately
        printf("Exiting from shell...\n");
        exit(0);
    }
    if (cheackInput(token) == 1) { //check first token
        printf("command %s not found\n" , token);
        return NULL;
    }

    parseInfo* info = (parseInfo*)malloc(sizeof(parseInfo));
    if (info == NULL){
        return NULL;
    }
    info[0] = strdup(token);
    int i=0;
    while(token != NULL){
        info[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
    info[i] = NULL; //last token
    return info;
}



void executeCommand(parseInfo* info){

   if (info == NULL){
        exit(0);
   }
    if (strcmp(info[0],"cd")==0){   
        handleCD(info); 
    }
    if ((strcmp(info[0],"cat")==0 && strcmp(info[1],">")==0)||strcmp(info[0],"nano")==0){
        handleNano(info);
    }
    else if (strcmp(info[0],"tree")==0 && info[1]==NULL){
        char current[1024];
        getcwd(current,sizeof(current));
        print_tree(current,0);
    }

    if (strcmp(info[0],"ls")==0 && 
        (info[1] != NULL) &&
        (info[2] != NULL) &&
        strcmp(info[1],"-l")==0 
        && strcmp(info[2],">")==0
        && haspipe(info)== 0 )
        {
            handleOutput(info);
            freeInfo(info);
        }
    if (haspipe(info)==1){
        handlePipe(info);
    }
    else {
        execvp(info[0],(char**)info);
        }

    freeInfo(info);
}

int checkNumOfTokens(char* line){
    int num_of_tokens=1;
    for(int j=0;line[j]!='\0';j++){ // count how many tokens in the line
        if (line[j] == ' ' )
            num_of_tokens++;
    }
    return num_of_tokens;
}


// returns 1 if [pipe] | in info
int haspipe(parseInfo* info)
{
    for(int i=0 ; info[i] != NULL; i++)
        if(strcmp(info[i],"|")==0){
            for(int j=i+1 ; info[j] != NULL; j++){
                if(strcmp(info[j],"|")==0){
                    printf("only one pipe supported!\n");
                    exit(1);
            }
            }
            return 1;
        }
    return 0;
}

int handlePipe(parseInfo* info){
    int flag = 0;
    parseInfo *p1,*p2;
    int i,j,k1,k2;
    int p_fd[2]; //for pipe command
    for(i=0; info[i] != NULL; i++){
        if (strcmp(info[i], "|") == 0 ) {
            flag = 1;
            p1 = (parseInfo*)malloc(sizeof(parseInfo));
            p2 = (parseInfo*)malloc(sizeof(parseInfo));

            for(j=0, k1=0,k2=0;info[j] != NULL;j++){
                if (j<i){ // copy to p1
                    p1[k1++] = strdup(info[j]);
                }
                if (j>i){ // copy to p2
                    p2[k2++] = strdup(info[j]);
                }
            }
            p1[k1] = NULL;
            p2[k2] = NULL;
        
        } 
    }
    if (flag == 1){//if there is pipe that needs to be handeled

        int check=pipe(p_fd);
        if(check != -1){

            int childPid = fork();
            if(childPid == 0)
            {
                close(p_fd[READ]);
                dup2(p_fd[WRITE], WRITE);
                executeCommand(p1);
            }
            else
            {
                waitpid(childPid,NULL,0);
                close(p_fd[WRITE]);
                dup2(p_fd[READ], READ);
                executeCommand(p2);
            }
        }
    }
    return flag;
}

int cheackInput(char* token){   //check the first word in line (check pipe/exit separately)
    return 
    !(strcmp(token, "pwd")== 0 || 
    strcmp(token, "cd" ) == 0 || 
    strcmp(token, "nano") == 0 || 
    strcmp(token, "cat") == 0 ||
    strcmp(token, "cp") == 0 || 
    strcmp(token, "wc") == 0 ||
    strcmp(token, "clear") == 0 || 
    strcmp(token, "grep") == 0 ||
    strcmp(token, "tree") == 0 ||
    strcmp(token, "ls")== 0 );
}

int checkWcFlags(char* token){
    return 
    !(strcmp(token, "-l")== 0 || 
    strcmp(token, "-c" ) == 0 || 
    strcmp(token, "-w") == 0 ||
    strcmp(token, "|" ) == 0 );
}

int checkGrepFlags(char* token){
    return !(strcmp(token, "-c" ) == 0 ||
    strcmp(token, "|" ) == 0);
}

int checkLsFlags(char* token){
    return !(strcmp(token, "-l" ) == 0 )||
    strcmp(token, "|" ) == 0;
}

void printPath(){
    char current[1024];
    getcwd(current,sizeof(current));
    printf("\033[0;35m%s\033[0m: %s", getenv("USER"),current);
 }

void handleCD(parseInfo* info){
    //for cd comends change directories
    if (info[1] != NULL && info[2] != NULL){
        printf("cd: too many arguments\n");
        return;
    }
    if(info[1] == NULL || strcmp(info[1],"~")==0)
        chdir(getenv("HOME"));
    else {
        int i= chdir(info[1]);
        if (i!=0){
            printf("no such file or directory\n");
        }
    }
    
}

void handleNano(parseInfo* info){
    //create new file for cat > and nano to save enter+ctrl D
    FILE *file;
    char temp[MAX];

    if (strcmp(info[0],"cat")==0 && strcmp(info[1],">")==0){
        if (info[2] == NULL){
            printf("cat >: requires a file name\n");
            freeInfo(info);
            exit(1);
        }
        file = fopen(info[2],"w");
    }

    else if (strcmp(info[0],"nano")==0){
        if (info[1] == NULL){
            printf("nano: requires a file name\n");
            freeInfo(info);
            exit(1);
        }
        file = fopen(info[1],"w");
    }

    if(!file)
    {
        printf("error openning file\n");
        freeInfo(info);
        exit(1);
    }    

    while(fgets(temp, MAX, stdin)) fprintf(file, "%s" ,temp);
    fclose(file);
    freeInfo(info);
    exit(0);
}

void handleOutput(parseInfo* info){ // handeling ls -l > output
    if (info[3]==NULL)
    {
        printf("ls -l: requires a file name\n");
        exit(1);
    }
    pid_t pid=fork();
    FILE *file;
    if (pid == 0){
        //child process
        file = fopen(info[3],"w");
        if(file == NULL){
            printf("error openning file\n");
            freeInfo(info);
            exit(1);
        }
        dup2(fileno(file),STDOUT_FILENO);
        fclose(file);
        execlp("ls", "ls" , "-l",NULL);
    } 
    else {
        //father
        waitpid(pid, NULL,0);
    }
}

void print_tree(char* path,int level) //recursive function to print tree
{
    DIR *dir;
    struct dirent *entry;

    if((dir = opendir(path))==NULL){
        return;
    }
    while ((entry=readdir(dir)) != NULL){
        if (strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0)
            continue;
        for(int i=0 ; i<level ; i++){
            printf("| ");
        }
        printf("|--%s\n",entry->d_name);
        char subpath[MAX];
        sprintf(subpath, "%s/%s",path,entry->d_name);
        
        struct stat sb;
        
        if(stat(subpath,&sb) == -1){
            fprintf(stderr, "error getting information about path\n");
            continue;
        }
        if(S_ISDIR(sb.st_mode)){
            print_tree(subpath,level+1);
        }
    }
    closedir(dir);
}

void freeInfo(parseInfo* info) // Free the allocated memory
{
    if(info){
        for(int i=0 ; info[i] != NULL ; i++)
            if(info[i])
                free(info[i]);
        free(info);
    }
}