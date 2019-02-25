#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <fcntl.h>

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
	int in = 0;
	int out = 0;
	tok = strtok_r(rest, " ", &rest);
	while(tok != NULL && i < MAXTOKS) {
		command[i] = tok;

		// detect if input contains input/output redirection
		if(strcmp(tok, "<") == 0) {
			in = i;
			i--;
		} else if(strcmp(tok, ">") == 0) {
			out = i;
			i--;
		}

		i++;
		tok = strtok_r(NULL, " ", &rest);
	}
	command[i] = NULL;

	// if commands equals 'cd', execute
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

		// check if input should be retrieved from file
		if(in) {
			int fd0;
			if((fd0 = open(command[in], O_RDONLY, 0)) == -1) {
				perror(command[in]);
				exit(EXIT_FAILURE);
			}
			dup2(fd0, 0);
			close(fd0);
			command[in] = NULL;
		}

		// check if output should be redirected
		if(out) {
			int fd1;
			if((fd1 = open(command[out], O_WRONLY | O_CREAT | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
				perror (command[out]);
				exit( EXIT_FAILURE);
			}
			dup2(fd1, 1);
			close(fd1);
			command[out] = NULL;
		}

		// execute command, if execvp returns, print error
		execvp(command[0], command);
		printf("msh: %s: %s\n", command[0], strerror(errno));
		exit(1);
	} else {
		wait(NULL);
	}

	return;
}
