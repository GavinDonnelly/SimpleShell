/***************************************************************************
 * File: SimpleShell.c
 *
 * Synopsis: Simple Shell built in c, to the spec: 
 *			 http://gavindonnelly.com/code-challenge/2015/4/21/simple-shell
 *
 * Authors:
 *      Gavin Donnelly
 *
 * Email:
 *		Gavin@GavinDonnelly.com
 *
 * Repo:
 *		https://github.com/GavinDonnelly/SimpleShell
 *********************************************************************/

 /*************************************************************************
 * Description of Implemention:
 *
 *
 *
 *
 *
 *
 *
 *
 **************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

 /* History */
typedef struct {
	int count, inUse;
	char *tokens[50];
} history_t;

/* Alias */
typedef struct {
	int comCount;
	char *ali[50];
	char *com[50];

} alias_t;

/* counter for aliases */
int aliList = 0;

int count = 0;

/* Change directory function */
void cd(char *tokens[]) {
	if(tokens[1] == NULL) 
        chdir(getenv("HOME"));
        
	else if(tokens[2] != NULL)
		printf("Too many arguments!\n");
		
	else if(chdir(tokens[1]) == -1) 
            printf(" %s: no such directory\n", tokens[1]);
}

/* get Path function */
void getPath() {
	printf("\n%s\n", getenv("PATH"));
}

/* Set path function */
void setPath(char *tokens) {
	setenv("PATH", tokens, 1);
}

/* Print current List of history of commands funchist[x]->counttion */
void pHistory(history_t *hist[]) {
	int x, y;
	
	printf("\n");
	for(x = 0; x < 20; x++) {
		if(hist[x]->tokens[0] == NULL)
			continue;
			
		printf("Command #%d:\t", hist[x]->count + 1);
		
		for(y = 0; y < 50; y++) {
			if(hist[x]->tokens[y] == NULL)
				break;
				
			printf("%s ", hist[x]->tokens[y]);
		}
		printf("\n");
	}
}

/* Set structures for history and alias, get original path, set directory to home and 
load alias */
char* startUp(history_t *hist[]) {
	int i, y;
	
	for(i = 0; i < 20; i++) {
		hist[i] = malloc(sizeof(history_t));
		hist[i]->count = i;
		hist[i]->inUse = 0;
	}

	chdir(getenv("HOME"));
	return strdup(getenv("PATH"));
}

/* Print current working directory function */
void pwd() {
	char wd[1024];
	
	if(getcwd(wd, sizeof(wd))!= NULL)
		printf("\n%s\n", wd);
		
	else
		puts(strerror(errno));
}

void flushInput(char in[]) {
	int c, j;
	
	for(j = 0; j < 513; j++) {
		if(in[j] == '\n')
			break;
		if(in[j] == ' ')
			break;
		else if(in[j] == '\0' && j == 512)
			while ((c = getchar()) != '\n'); /* Flush leftover input */
	}
}

/* fork function */
void execOther(history_t *hist[], int count) {
	pid_t pid;
	
	pid = fork();
		
	if(pid == 0) { /* child process */
		if(execvp(hist[count]->tokens[0], hist[count]->tokens) == -1) {
			puts(strerror(errno));
			exit(127);
		}
		exit(0);
	}
	else if(pid < 0) { /* failed to fork */
		printf("\nThe process failed to fork properly\n");
		exit(1);
	}
	else { /* parent process waits for child process to terminate */
		wait(0);
	}
}

/* check users command for exactable function commands, otherwise fork */
void execCommand(history_t *hist[], int count) {
	char *number;
	if(hist[count]->tokens[0] != NULL) {
		
		if(strcmp(hist[count]->tokens[0], "cd") == 0) 
			cd(hist[count]->tokens);
			
		else if(strcmp(hist[count]->tokens[0], "pwd") == 0) 
			pwd();
			
		else if(strcmp(hist[count]->tokens[0], "!!") == 0) {
			if(count != 0)
				execCommand(hist, count - 1);
				
			else if(hist[19]->inUse == 1)
				execCommand(hist, 19);
		}
		
		else if(hist[count]->tokens[0][0] == '!') {
			number = strtok(hist[count]->tokens[0], "! \t\n");
			
			if(*number - '0' >= 1 && *number - '0' <= 20) 
				execCommand(hist, *number - '0' - 1);
			else
				printf("\nIncorrect formatting: !<no>, where no is between 1 and 20\n");
		}
		
		else if(strcmp(hist[count]->tokens[0], "history") == 0)
			pHistory(hist);
			
		else if(strcmp(hist[count]->tokens[0], "getpath") == 0)
			getPath();
			
		else if(strcmp(hist[count]->tokens[0], "setpath") == 0)
			setPath(hist[count]->tokens[1]);
		
		else 
			execOther(hist, count);
	}
}

/* Read Input and deal with appropriately */
int readInput(history_t *hist[], char in[], int count, int i) {
	char *tok, *tokens[50];
	int a;
	
	if(in[0] == '\n') {
		printf("\nPlease input a command.\n");
		return -3;
	}
	
	tok = strtok(in, " \t\n");

	if(tok != NULL)
		tokens[i++] = strdup(tok);
		
	while(tok != NULL) {
		if(strcmp(tok, "exit") == 0)
			return -1;
			
		tok = strtok(NULL, " \t\n");
		
		if(tok != NULL)
			tokens[i++] = strdup(tok);
	}
	
	tokens[i] = NULL;
	
	if(tok != NULL)
		if(strcmp(tok, "exit") == 0)
			return -1;

	if(tok != NULL)
		if(strcmp(tok, "exit") == 0)
			return -1;
			
	if(strcmp(tokens[0], "!!") == 0) {
		if(count != 0)
			return count - 1;
			
		else if(hist[19]->inUse == 1)
			return 19;
			
		else if(hist[19]->inUse == 0) {
			printf("\nYou do not have any previous commands to invoke.\n");
			return -3;
		}
	}

	else if(tokens[0][0] == '!' && i == 1) {
		tok = strtok(tokens[0], "! \t\n");
		if(*tok - '0' >= 1 && *tok - '0' <= 20) {
			if(hist[*tok - '0' - 1]->inUse == 1)
				return *tok - '0' - 1;
				
			else {
				printf("\nThere have been less than %d commands entered.\n", *tok - '0');
				return -3;
			}
		}
		else
			printf("\nIncorrect formatting: !<no>, where no is between 1 and 20\n");
	}

	for(a = 0; a < 50; a++) {
		if(tokens[a] == NULL) {
			hist[count]->tokens[a] = NULL;
			break;
		}
		hist[count]->tokens[a] = strdup(tokens[a]);
	}
	return -2;

}

 /* Main function */
int main(int argc, char *argv[]) {
	int i, inputResult;
	char *path = "", str[513];
	history_t *history[20] = { NULL };
	
	path = startUp(history);
	do {
		i = 0;
		printf("\n>");
			
		if(fgets(str, 513, stdin) == NULL) 
			break;
		
		flushInput(str);
		inputResult = readInput(history, str, count, i);
		
		if(inputResult == -1)
			break;
		
		else if(inputResult == -2) {
			execCommand(history, count);
			history[count]->inUse = 1;
			count = (count + 1) % 20;
		}

		else if(inputResult == -3)
			continue;

		else if(inputResult >= 0)
			execCommand(history, inputResult);
	
	} while(1);
	printf("\n");
	chdir(getenv("HOME"));
	setenv("PATH", path, 1); 
	
	return 0;
}