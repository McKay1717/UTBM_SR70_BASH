
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "color.h"
#include "shell-utils.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"

#define INPUT_BUFFER_SIZE 2048
#define NB_MAX_TOKENS 512



void INThandler(int sig){
    char c;
    printf("\nDo you want to quit [y/n] ? ");
    c = getchar();
    if(c=='y' || c=='Y'){
        exit(0);
    }
    
}


void exec_cmd(char* data){
    
    
    int return_status;
    int pid;
    if((pid=fork())==0){
	int nb_tokens;
	char* tokens[NB_MAX_TOKENS+1];
    nb_tokens=split_tokens(tokens, data, NB_MAX_TOKENS);
    char** last_token;
    char** new_token;
    int com[2];

   	if (nb_tokens==NB_MAX_TOKENS) {
	    	fprintf(stderr, "Too many tokens: exiting\n");
	    	exit(3);
    	}

	    if (nb_tokens<=0) {
	    	fprintf(stderr, "Cannot split tokens: exiting\n");
	    	exit(4);
    	}   
        
        if(strcmp(tokens[0],"exit")==0){
            fprintf(stdout,ANSI_COLOR_RED "Goodbye...\n" ANSI_COLOR_RESET);
            exit(0);
        }
		
    last_token=tokens;
    for(;;){
        new_token=trouve_tube(last_token,"|");
        if(new_token==NULL){
            break;
        }
        if(pipe(com)<0){
            perror("pipe");
            return;
        }
        if((pid=fork())==0){
            //Fils
            close(com[0]);
            dup2(com[1],1);
	        execvp(last_token[0], last_token);
            close(com[1]);
            exit(0);
        }else{
            //Father
            close(com[1]);
            dup2(com[0],0);
            waitpid(pid,&return_status,0);
            close(com[0]);
        }
        last_token=new_token;
    }


	    if(execvp(last_token[0], last_token)<0){
            perror("execvp");
            exit(-1);
         }
    }else{
       waitpid(pid,&return_status,0);
    }
}

int main() {
	char line[INPUT_BUFFER_SIZE+1];

	char* data;

    //recuperation login
    
    char login[25];
    getlogin_r(login,25);
 
    signal(SIGINT,INThandler);

	for(;;){

		data="";
        fprintf(stdout,ANSI_COLOR_GREEN "%s : " ANSI_COLOR_RESET,login);
	    data=fgets(line, INPUT_BUFFER_SIZE, stdin);

    	if (data==NULL) {
		/* Erreur ou fin de fichier : on quitte tout de suite */
	    	if (errno) {
    			perror("fgets");
    		} else {
    			fprintf(stderr, "EOF: exiting\n");
    		}
	    	exit(1);
    	}

    	if (strlen(data) == INPUT_BUFFER_SIZE-1) {
	    	fprintf(stderr, "Input line too long: exiting\n");
    		exit(2);
    	}


 
        exec_cmd(data);
	}

    exit(0);
}
