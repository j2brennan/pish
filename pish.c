#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// You may not include additional header files.

#include "pish_history.h"
#define MAX_COMMAND_LENGTH 256

// Prototypes
void run(struct pish_arg *arg); 

/*
 * Script mode flag. If set to 0, the shell reads from stdin. If set to 1,
 * the shell reads from a file from argv[1].
 */
static int script_mode = 0;

/*
 * List of built-in commands for the pish shell.
 */
static char *built_in_commands[3] = {"exit", "cd", "history"};
static int commands_array[3] = {0,1,2}; 

/*
 * Prints a prompt IF NOT in script mode (see script_mode global flag).
 */
void prompt(void) {
    if (!script_mode) {
        char *working_dir = getcwd(NULL, 0);
        struct passwd *user = getpwuid(getuid());
#ifdef PISH_AUTOGRADER
        printf("%s@pish %s$\n", user->pw_name, working_dir);
#else
        printf("\e[0;35m%s@pish \e[0;34m%s\e[0m$ ", user->pw_name, working_dir);
#endif
        fflush(stdout);
        free(working_dir);
    }
}

void usage_error(void) {
    fprintf(stderr, "pish: Usage error\n");
    fflush(stderr);
}

/*
 * Break down a line of input by whitespace, and put the results into
 * a struct pish_arg to be used by other functions.
 *
 * @param command   A char buffer containing the input command
 * @param arg       Broken down args will be stored here
 */
void parse_command(char *command, struct pish_arg *arg) {
	printf("Parsed command: %s\n", command);
	printf("ARGC PARSE COMMAND: %d\n", arg -> argc);
	printf("ARGV: %d\n", arg -> argc);
	int cmd;
	for(int i = 0; i < sizeof(built_in_commands)/sizeof(built_in_commands[0]); i++) {
		if(strcmp(built_in_commands[i], arg -> argv[0]) == 0){
			cmd = i;
		}
	}

	switch(cmd) {
		case 0:
			printf("Run EXIT");
			break;
		case 1:
			printf("Run CD");
			break;
		case 2:
			printf("Run HISTORY");
			break;
		default:
			printf("Run command with run()");
			break;
	}		
}

/*
 * Run a command.
 *
 * Built-in commands are handled internally by the pish program.
 * Otherwise, use fork/exec to create child processes to run the program.
 *
 * If the command is empty, do nothing.
 * If NOT in script mode, add the command to history file.
 */
void run(struct pish_arg *arg) {
	char *cmd = arg->argv[0];

	if( cmd == NULL ) {
    		printf("RUN COMMAND: %s\n", cmd);
	}

	/*
	 * Create a child processes
	 */

	pid_t id = fork();

	/*
	 * After the fork, check if the parent process is running, wait.
	 * If the child process is running execute the command.
	 */
	
	if(id ==0) {
		printf("Child process");
		execvp(cmd, arg->argv);
	} else if(id > 0) {
		wait(NULL);
	}
	// execvp(tokens[0], tokens);

}

/*
 * The main loop of pish. Repeat until the "exit" command or EOF:
 * 1. Print the prompt
 * 2. Read command from fp (which can be stdin or a script file)
 * 3. Execute the command
 *
 * Assume that each command never exceeds MAX_COMMAND_LENGTH-1 chars.
 */
int pish(FILE *fp) {
    // TODO
	char buffer[50];
	if(!fp) {
		perror("Error opening file");
		return -1;
	}
	
	fread(buffer, sizeof(char), 50, fp);


	int index = 0;
	while(1 == 1) {
	char *command = strtok(buffer, "\n");

	struct pish_arg *arg;
	int j = 0;
	while( command != EOF ) {

	char *token = strtok(command, " ");

	int i = 0;
	while( token != NULL ) {
		arg->argv[i] = token;
		arg->argc = i + 1;
		token = strtok(NULL, " ");
		i++;
	}




		printf("COMMAND: %s", command);
		command  = strtok(NULL, "\n");
		j++;
	}
	// parse_command(command,arg); 
	//for(int i = 0; i < 50; i++) {
	 //      printf("Element %d: %c\n", i + 1, buffer[i]);	
	//}
	
	printf("Parsed command: %s\n", command);
        printf("ARGC PARSE COMMAND: %d\n", arg -> argc);
        printf("ARGV: %d\n", arg -> argc);
        int cmd;
        for(int i = 0; i < sizeof(built_in_commands)/sizeof(built_in_commands[0]); i++) {
        	if(strcmp(built_in_commands[i], arg -> argv[0]) == 0){
        		cmd = i;
        	}
        }
                                                                                          
        switch(cmd) {
        	case 0:
        		printf("Run EXIT");
        		break;
        	case 1:
        		printf("Run CD");
        		break;
        	case 2:
        		printf("Run HISTORY");
        		break;
        	default:
        		printf("Run command with run()");
			run(arg);	
        		break;
        }		
		arg->argv[index] = NULL;
	}
	fclose(fp);

	printf("OPENED FILE");
	//prompt();
	

    return 0;
}

/*
 * The entry point of the pish program.
 *
 * - If the program is called with no additional arguments (like "./pish"),
 *   process commands from stdin.
 * - If the program is called with one additional argument
 *   (like "./pish script.sh"), process commands from the file specified by the
 *   additional argument under script mode.
 * - If there are more arguments, call usage_error() and exit with status 1.
 */
int main(int argc, char *argv[]) {

	printf("ARGC: %d\n", argc);
	int arg_count = 0;
	
	if (argc > 2) {
		usage_error();
	} else if (argc == 2) {
		printf("Open Script Mode\n");
		char *file_name = argv[1];
		printf("FILE: %s", file_name);
		char *tok = strtok(file_name, ".");
		tok = strtok(NULL, " ");
		char *file_ext;
		file_ext = tok;
		printf("TOKEN: %s", tok);
		if (*file_ext == 's' && *(file_ext + 1) == 'h') {
			FILE *fptr;
			fptr = fopen("script.sh", "r");
			pish(fptr);
		} else {
			printf("Invalid file input %s", file_name);
		}
	} else if (argc == 1) {
		printf("Open in CLI Mode\n");
		//pish();
		prompt();
	}	       

    // constantly parse through the inputs
    // tokenize them in to commands
    // execute those commands
    //

   // char *tokens[64]; 
   // char buff[50];
   // char *ptr = NULL;
    //char *token = NULL;
  //  int index = 0;

    //while( 1 == 1 ) {
//	index = 0;
//	printf("psh >> ");    
  //  	fgets(buff, 50, stdin);
//	ptr = buff;
	
//	while((token = strtok(ptr, " ")) != NULL){

//		tokens[index] = token;
//		printf("%s\n", token);
//		index++;
//		ptr = NULL;
//	}
//	tokens[index] = NULL;
//	tokens[index-1][2] = '\0';

//	pid_t id = fork();

//	if(id ==0) {
//		execvp(token[0], tokens);
//	} else if(id > 0) {
//		wait(NULL);
//	}
	// execvp(tokens[0], tokens);
    return EXIT_SUCCESS;
}
