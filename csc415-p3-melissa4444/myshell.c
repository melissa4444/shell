/****************************************************************
 * Name        :  Melissa Estrada                               *
 * Class       :  CSC 415                                       *
 * Date        :  July 2019                                     *
 * Description :  Writting a simple bash shell program          *
 *                that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

/* CANNOT BE CHANGED */
#define BUFFERSIZE 256
/* --------------------*/
#define PROMPT "myShell >> "
#define PROMPTSIZE sizeof(PROMPT)

void pipes(char *argv1[], char *argv2[]){
    int pipedf[2];
    int sts= pipe(pipedf);
    pid_t p1, p2;
    
    if (pipe(pipedf)<0) {
        perror("Pipe failed not an init");
        exit(0);
    }
    p1=fork();
          if (p1==-1) {
              perror("failed fork");
              exit(0);
              }
                if (p1==0) {
                    close(pipedf[1]);
                    close(0);
                    dup(pipedf[0]);
                    
                
                if (execvp(argv2[0], argv2) < 0) {
                    perror("command error");
                    exit(0);
                }
                close(pipedf[0]);
                
                }
                p2=fork();
                if(p2==-1){
                    perror("failed fork");
                    exit(-1);
                    
                }
                if (p2==0) {
                    close(pipedf[0]);
                    close(1);
                    dup(pipedf[1]);
                    if (execvp(argv1[0], argv1)<0) {
                        perror("comand error");
                        exit(0);
                        
                        
                    }
                    close(pipedf[1]);
                    
                }
    close(pipedf[0]);
    close(pipedf[1]);
    while(wait(&sts)!=p2);
    while (wait(&sts) !=p1);
    
    
    
}


void execute(char *argv1[], int filename, int inputfile, int startBkg){
    int sts;
    pid_t pd;
    pd=fork();
    
    if (pd<0) {
        perror("failed fork");
            exit(0);
        }
        else if(pd==0){
            if (execvp(argv1[0], argv1)<0) {
                perror("invalid command");
                exit(1);
            }
            if (filename) {
                close(1);
                dup(filename);
            }
            if (inputfile) {
                close(0);
                dup(filename);
            }
            if (filename) {
                close(filename);
            }
            if (inputfile) {
                close(inputfile);
            }
        }
        else{
            if(!startBkg){
                while(wait(&sts)!=pd);
            }
        }
    }



int 
main()
{
    while(1){
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        printf("%s: %s", PROMPT, cwd);
    int arg1=0;
    int arg2=0;
    int filename=0;
    int startBkg=0;
    int inputfile=0;
    int pipe=0;
    int temp=0;
    char *modifier;
    char buffer[BUFFERSIZE];
    char bufferc[BUFFERSIZE];
    
    if (fgets(buffer, BUFFERSIZE, stdin)) {
        strncpy(bufferc, buffer, BUFFERSIZE);
    }
    else{
        continue;
    }
    modifier=strtok(bufferc," \n");
    
    while (modifier != NULL) {
        if (strcmp(modifier, "&")==0) {
            startBkg=1;
        }
        
        else if(strcmp(modifier,">")==0){
            modifier=strtok(NULL," \n");
            
            filename=open(modifier, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            
            if (filename==1) {
                perror("invalid file, can not open file");
                exit(0);
            }
            
        }
        else if(strcmp(modifier,">>")==0){
            modifier=strtok(NULL," \n");
            filename=open(modifier, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            
            if (filename==-1) {
                perror("invalid file, can not open file");
                exit(0);
            }
        }
        else if(strcmp(modifier, "<")==0){
            modifier=strtok(NULL, " \n");
            inputfile=open(modifier, O_RDONLY, NULL);
            exit(0);
            
            if (filename==-1) {
                perror("invalid file, can not open");
                exit(0);
            }
        }
        else if(strcmp(modifier, "|")==0){
            pipe=1;
            temp=arg1;
            arg1=0;
        }
        else{
            ++arg1;
        }
        modifier=strtok(NULL," \n");
        
    }
    if (arg1 !=0) {
        if (pipe) {
            arg2=arg1;
            arg1=temp;
        }
        char *arg1Array[arg1+1], *arg2Array[arg2+1];
        arg1Array[0]=strtok(bufferc, "\n");
        int i=0, j=0;
        do{
            if (startBkg) {
                if (strcmp(arg1Array[i], "&")==0) {
                    arg1Array[i]=strtok(NULL," \n");
                    break;                }
            }
            if (filename) {
                if (strcmp(arg1Array[i], ">")==0) {
                    strtok(NULL, " \n");
                    arg1Array[i]=strtok(NULL, " \n");
                    break;
                    
                }
                else if(strcmp(arg1Array[i], ">>")==0){
                    strtok(NULL, " \n");
                    arg1Array[i]=strtok(NULL, " \n");
                    break;
                }
            }
            
            
            if (inputfile) {
                if (strcmp(arg1Array[i], "<")==0) {
                    strtok(NULL, " \n");
                    arg1Array[i]=strtok(NULL, " \n");
                    break;
                }
            }
            
            
            if (pipe) {
                if (strcmp(arg1Array[i], "|")==0) {
                    do{
                        arg2Array[j]=strtok(NULL, " \n");
                    }
                    while(arg2Array[++j] !=NULL);
                    arg1Array[i]=arg2Array[--j];
                    break;
                }
            }
            
            arg1Array[++i]=strtok(NULL, " \n");
        }
        while (arg1Array[i] != NULL);
            if (strcmp(arg1Array[0], "exit")==0) {
                exit(0);
            }
            
            else if(strcmp(arg1Array[0], "cd")==0){
                if (arg1Array[1]==NULL) {
                    chdir(getenv("HOME"));
                }
                else{
                    chdir(arg1Array[1]);
                }
            }
            
            else{
                if (pipe) {
                    pipes(arg1Array, arg2Array);
                }
                else{
                    execute(arg1Array, filename, inputfile, startBkg);
                }
            }
        }
    }
    
    
    
return 0;
}














/*
 RESOURCES:
 https://www.geeksforgeeks.org/making-linux-shell-c/
 https://www.geeksforgeeks.org/fgets-gets-c-language/
 https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
 https://www.tutorialspoint.com/c_standard_library/c_function_strncpy
 https://linux.die.net/man/3/execvp
         provide an array of pointers to null-terminated strings that represent the argument list available to the new program. The first argument, by convention, should point to the filename associated with the file being executed. The array of pointers must be terminated by a NULL pointer.
 */
