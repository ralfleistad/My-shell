#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>

// max length input string
#define MAXSTR 120
#define MAXTOKS 100

// function prototypes
void processInput(char *input, int arguments);
bool standardOutput(char *input);

int main(int argc, char **argv) {

	char s[MAXSTR + 3];

	// Execute if arguments are provided when running the program
	if(argc > 1) {
		FILE *file = fopen(argv[1], "r");
		char fileline[MAXSTR] = {0};

		// While there are lines in the file, keep processing
		while(fgets(fileline, MAXSTR, file)) {
			processInput(fileline, argc);
		}
		exit(0);
	}


	while(true) {

		//prompt
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
			processInput(s, 0);
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

void processInput(char *input, int arguments) {

	// remove newline from the input
	char *pos;
	if((pos = strchr(input, '\n')) != NULL) {
		*pos = '\0';
	}

	// check if input is a predefined command
	if(standardOutput(input)) {
		return;
	}

	// Tokenize the input	
	char *command[MAXTOKS];
	char *rest = input;
	char *tok;
	int i = 0;
	while((tok = strtok_r(rest, " ", &rest)) != NULL && i < MAXTOKS) {
		command[i] = tok;
		i++;
	}
	command[i] = NULL;

	if(strcmp(command[0], "cd") == 0) {
		char *dir = command[1];
		int ret;
		const char *home = getenv("HOME");

		if(dir == NULL) {
			ret = chdir(home);
		} else {
			ret = chdir(dir);
		}

		if(ret == -1) {
			printf("msh: %s: %s\n", command[0], strerror(errno));
		}
		return;
	}

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
