#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void shell_lopp(void);
char *read_line(void);
char **parse_line(char *line);
int exec_process(char **args);

int next = 0;

int main(){
	while(1){
		printf("> ");
		char *line;
		char **args;
		int status;

		line = read_line();
		args = parse_line(line);
		status = exec_process(args);

		next = 0;
		free(line);
		free(args);
	}
	return 0;
}

char *read_line(void){
	char *buffer = malloc(sizeof(char) * 1024);
	int index = 0;

	while(1){
		char c = '\0';
		scanf("%c", &c);
		if(c == EOF || c == '\n'){
			buffer[index] = '\0';
			return buffer;
		}
		else{
			buffer[index] = c;
		}
		index++;
	}
}

char **parse_line(char *line){
	char **tokens = malloc(1024 * sizeof(char*));
  	char *token;
  	int index = 0;

	char *delim = " \r\n";
  	token = strtok(line, delim);
	while (token != NULL) {
		tokens[index] = token;
		index++;
		token = strtok(NULL, delim);
	}
	if(index > 0)
		if( strcmp(tokens[index-1], "&") == 0 ){
			next = 1;
			tokens[index-1] = NULL;
		}
	tokens[index] = NULL; // last one must be NULL
	return tokens;
}

int exec_process(char **args){
	pid_t pid, pwait;

	pid = fork();
	if(pid == 0){
		// Child process
		if(next == 1){
			if(fork() == 0){
				// Child Child process
				if (execvp(args[0], args) == -1) {
					perror("Error!");
				}
			}
			else
				exit(0);
		}
		else{
			if(execvp(args[0], args) == -1) {
				perror("Error!");
			}
		}
		exit(0);
	}
	else if(pid < 0){
		// Error
		printf("Error forking!\n");
	}
	else{
		// Parent process
		pwait = wait(NULL);
	}
	return 0;
}


