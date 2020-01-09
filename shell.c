#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAXLEN  100

//#define DEBUG	0x1
#ifdef DEBUG
#define DBG_PRINT(...) fprintf( stdout, __VA_ARGS__ )
#else
#define DBG_PRINT(...) do{ } while ( 0 )
#endif

#define IS_CHILD(pid)   pid == 0
#define CLEAR() printf("\033[H\033[J")
#define PRINT_PROMPT(cwd)  printf("%s:psh > ", cwd)

/*
 * Initialize the shell
 * 1. Clear the screen
 * 2. Init cmd_list
 */
void init_shell(char **cmd_list)
{
	int i;

	CLEAR();
	for ( i = 0; i < MAXLEN; i++)
		cmd_list[i] = NULL;
	
	return;
}

/*
 * Get the current directory
 * and the current user info
 * Display along with the prompt
 */
void print_shell_prompt()
{
	char *ret;
	char cwd[_POSIX_PATH_MAX];
	
	ret = getcwd(cwd, _POSIX_PATH_MAX);
	if (ret == NULL) {
		perror("getcwd failed");
		return;
	}
	PRINT_PROMPT(cwd);

	return;
}

char *scan_shell_input()
{
	char *buf;
	size_t maxsize = 0;

	// if size == 0, malloc happens inside
	getline(&buf, &maxsize, stdin);
	buf[strlen(buf)-1] = '\0';

	return buf;
}

int process_shell_input(char *cmd, char **cmd_list)
{
	int i = 0;

	cmd_list[i] = strtok(cmd, " ");
	while (cmd_list[i] != NULL) {
		DBG_PRINT("cmd_list[%d]: %s\n", i, cmd_list[i]);
		i++;
		cmd_list[i] = strtok(NULL, " ");
	}
	
	return 0;
}

void execute_shell_input(char *cmd, char **cmd_list)
{
	int pid;

	pid = fork();

	if (pid == -1) {
		perror("Unable to fork a child");
		return;
	}

	if (IS_CHILD(pid)) {
		DBG_PRINT("This is child\n");

		if (execvp(cmd_list[0], cmd_list)) {
			perror("Command failed due to");
			exit(EXIT_FAILURE);
		} else {
			exit(EXIT_SUCCESS);
		}
	} else {
		wait(NULL);
		DBG_PRINT("Hi Im parent!\n");
	}

	return;
}
int main()
{
	int err;
	char *cmd;
	char *cmd_list[MAXLEN];


	init_shell(cmd_list);
	

	while (1) {
		
		print_shell_prompt();

		cmd = scan_shell_input();

		if (strcmp("exit", cmd) == 0) {
			break;
		}

		DBG_PRINT("PID: %d  Command %s\n", getpid(), cmd);

		err = process_shell_input(cmd, cmd_list);

		if (err)
			continue;

		execute_shell_input(cmd, cmd_list);

		free(cmd);
	}
	return 0;
}