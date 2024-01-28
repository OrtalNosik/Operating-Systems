#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

#define READ 0
#define WRITE 1
#define MAX 1024

typedef char* parseInfo;

parseInfo* parse(char* line); 

void executeCommand(parseInfo* tokens);

int cheackInput(char* token);
int haspipe(parseInfo* info);
int handlePipe(parseInfo* info);
int checkNumOfTokens(char* line);
int checkLsFlags(char* token);
int checkGrepFlags(char* token);
int checkWcFlags(char* token);
void printPath();
void handleCD(parseInfo* info);
void handleNano(parseInfo* info);
void print_tree(char* path,int level);
void handleOutput(parseInfo* info);
void freeInfo(parseInfo* info);
