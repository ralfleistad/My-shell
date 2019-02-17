#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// max length input string
#define MAXSTR 120

void processInput(char *input);

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
			char *buffer;
			if(buffer = strchr(s, '\n')) {
				*buffer = 0;
			} else {
				scanf("%*[^\n]");
				scanf("%*c");
			}

		} else {
			processInput(s);
		}

		
	} // while looip

	return 0;
} // int main

void processInput(char *input) {

   if(strcmp(input, "exit\n") == 0) {
      exit(0);
	}

	printf("%s", input);

}
