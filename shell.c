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
#include <sys/stat.h>
#include <fcntl.h>

#define INPUT_BUFFER_SIZE 2048
#define NB_MAX_TOKENS 512

static pid_t gpid = 0;
static int received = 0;
static pid_t last_exec = 0;

void INThandler(int sig) {
	char c;
    if(last_exec!=0)
        kill(last_exec,SIGTSTP);
    else{
        printf("\nDo you want to quit [y/n] ? ");
    	c = getchar();
    	if (c == 'y' || c == 'Y') {
	    	exit(0);
         }
    }

}
void FatherSTPhandler(int sig) {
	received = 1;
	if (last_exec != 0){
		kill(gpid, SIGTSTP);
        gpid=last_exec;
    }
}

void exec_cmd(char* data) {

	int return_status;
	int pid;
	int nb_tokens;
	char* tokens[NB_MAX_TOKENS + 1];
	nb_tokens = split_tokens(tokens, data, NB_MAX_TOKENS);
	char** last_token;
	char** new_token;
	int com[2];

	if (nb_tokens == NB_MAX_TOKENS) {
		fprintf(stderr, "Too many tokens: exiting\n");
        return;
	}

	if (nb_tokens <= 0) {
		fprintf(stderr, "Cannot split tokens: exiting\n");
        return;
	}

	if (strcmp(tokens[0], "exit") == 0) {
		fprintf(stdout, ANSI_COLOR_RED "Goodbye...\n" ANSI_COLOR_RESET);
		exit(0);
	} else if (strcmp(tokens[0], "fg") == 0){
		kill(gpid, SIGCONT);
        last_exec=gpid;
        gpid=0;
		waitpid(last_exec, &return_status, WUNTRACED | WCONTINUED);// voir avec nico
        last_exec = 0 ;
        return;
    }
     else if(strcmp(tokens[0],"cd")==0){
      if(chdir(tokens[1])<0){
          perror("chdir");
      }
      return;
     };

	if ((pid = fork()) == 0) {
		last_token = tokens;
		for (;;) {
			new_token = trouve_tube(last_token, "|");
			if (new_token == NULL) {
				break;
			}
			if (pipe(com) < 0) {
				perror("pipe");
				return;
			}
			if ((pid = fork()) == 0) {
				//Fils
				close(com[0]);
				dup2(com[1], 1);
				execvp(last_token[0], last_token);
				close(com[1]);
				exit(0);
			} else {
				//Father
				close(com[1]);
				dup2(com[0], 0);
				waitpid(pid, &return_status, 0);
				close(com[0]);
			}
			last_token = new_token;
		}
		char *in, *out; //Fichiers de redirection entre/sortis

		in = trouve_redirection(last_token, "<");
		if (in != NULL) {
			freopen(in, "r", stdin);
		}
		out = trouve_redirection(last_token, ">");
		if (out != NULL) {
			freopen(out, "w", stdout);
		}

		if (execvp(last_token[0], last_token) < 0) {
			perror("execvp");
		}
	} else {
        last_exec = pid;
		waitpid(pid, &return_status, WUNTRACED | WCONTINUED);
        last_exec = 0 ;
	}
}

int main() {
	char line[INPUT_BUFFER_SIZE + 1];

	char* data;
    char pwd[1024];
	//recuperation login

	char login[25];
	getlogin_r(login, 25);

	signal(SIGINT, INThandler);
	signal(SIGTSTP, FatherSTPhandler);

	for (;;) {
        getcwd(pwd,sizeof(pwd));
		data = "";
		fprintf(stdout, ANSI_COLOR_GREEN "%s:%s$ " ANSI_COLOR_RESET, login,pwd);
		data = fgets(line, INPUT_BUFFER_SIZE, stdin);

		if (data == NULL) {
			/* Erreur ou fin de fichier : on quitte tout de suite */
			if (errno) {
				perror("fgets");
			} else {
				fprintf(stderr, "EOF: exiting\n");
			}
			exit(1);
		}

		if (strlen(data) == INPUT_BUFFER_SIZE - 1) {
			fprintf(stderr, "Input line too long: exiting\n");
			exit(2);
		}

		exec_cmd(data);
	}

	exit(0);
}
