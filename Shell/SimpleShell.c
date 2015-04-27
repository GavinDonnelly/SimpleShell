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

/* Set structures for history and alias, get original path, set directory to home and 
load alias */
char* startUp(history_t *hist[], alias_t *alias[]) {
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
}

 /* Main function */
int main(int argc, char *argv[]) {
	int i, inputResult;
	char *path = "", str[513];
	
	path = startUp(history, alias);
	do {
		i = 0;
		printf("\n>");
			
		if(fgets(str, 513, stdin) == NULL) 
			break;
		
		flushInput(str);
		inputResult = readInput(history, str, count, i);
		
		if(inputResult == -1)
			break;
		
		else if(inputResult == -3)
			continue;
	
	} while(1);
	printf("\n");
	chdir(getenv("HOME"));
	setenv("PATH", path, 1); 
	
	return 0;
}