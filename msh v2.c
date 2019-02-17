#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>

// max length input string
#define MAXSTR 120

// function prototypes
void processInput(char *input);
bool standardOutput(char *input);

int main() {

	char s[MAXSTR + 3];

	while(true) {

		// prompt
		printf("msh> ");
		if(fgets(s, MAXSTR + 3, stdin) == NULL) {
			break;
		}

		// check input length; n does not include final carriage return
		int n = strlen(s) - 1;  
		if (n > MAXSTR) {
			printf("error: input too long\n");
			
			// if input was too long, clear the fgets buffer
			char* buffer;
			if(buffer = strchr(s, '\n')) {
				*buffer = 0;
			} else {
				scanf("%*[^\n]");
				scanf("%*c");
			}

		} else {
			processInput(s);
		}	
	}

	return 0;
}

// function for predefined commands
bool standardOutput(char *input) {
	
	if(strcmp(input, "") == 0) { 
		return true;
	}

   if(strcmp(input, "exit") == 0) {
      exit(0);
	}

	if(strcmp(input, "help") == 0) {
		printf("enter Linux commands, or ‘exit’ to exit\n");
		return true;
	}

	if(strcmp(input, "today") == 0) {
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		char s[12];

		strftime(s, sizeof(s), "%m/%d/%Y", tm);

		printf("%s\n", s);		
		return true;
	}

	return false;
}

// tokenize and process input
void processInput(char *input) {

	// remove newline from the input
	char* newline = strchr(input, '\n');
	if(newline) {
		*newline = 0;
	}

	// check if input is a predefined command
	if(standardOutput(input)) {
		return;
	}

	// Tokenize the input	
	char* tokens = strtok(input, " "); 
	char* command[MAXSTR + 2];
	int i = 0;
	
	// add tokens to array
	while(tokens) {
		command[i++] = tokens;
		tokens = strtok(NULL, " ");
	}
	command[i] = NULL;

	// fork and execute the given command / print error
	int rc = fork();
	if(rc < 0) {
		printf("fork failed\n");
	} else if (rc == 0) {
		execvp(command[0], command);
		printf("msh: %s: %s\n", command[0], strerror(errno));
		exit(1);
	} else {
		wait(NULL);
	}

	return;
}
