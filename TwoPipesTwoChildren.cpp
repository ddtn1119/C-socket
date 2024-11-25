// Name: Andy Nguyen
// Course: CS3377
// Started date: 1 April 2024
// Assignment 5, Part 1.1
// 	purpose of this program: to execute "ls -ltr | grep 3376 | wc -l" using 2 pipes, 2 children
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv){
	// initialise PIDs of children
	pid_t childpid1, childpid2; 
	
	// note: I received multiple warnings about converting char* to string during the compilation of this program
	// shell equivalence: "ls -ltr | grep 3376 | wc -l
	char *ls_args[] = {"ls", "-ltr", NULL}; // initialise ls argument
	char *grep_args[] = {"grep", "3376", NULL}; // initialise grep argument
	char *wc_args[] = {"wc", "-l", NULL}; // initialise wc argument

	// create one pipe to send the output of ls process to grep (first child)
	// create another pipe to send the output of grep process to wc (second child)
	int p1[2], p2[2]; // index 0 = stdin; index 1 = stdout;
	pipe(p1);
	pipe(p2);

	// fork the first child to execute ls
	childpid1 = fork();
	if(childpid1 < 0){
		perror("Error: failed to create a child process");
		exit(1); // exit failure 
	}
	else if(childpid1 == 0){ 
		// replace ls's stdout with the write part of 1st pipe
		dup2(p1[1], 1);
		// close all pipes (this is important because descriptors are shared between the parent and the child, if we don't close the pipes then the child will never reach the end of file (EOF) thus will never exit) 
		close(p1[0]);
		close(p1[1]);
		close(p2[0]);
		close(p2[1]);
		execvp(*ls_args, ls_args); // execute ls
		exit(0); // exit after executing commands
	}
	else{
		// now fork the second child to execute grep
		childpid2 = fork();
		if(childpid2 < 0){
			perror("Error: failed to create a child process");
			exit(1);
		}
		else if(childpid2 == 0){
			// replace grep's stdin with the write part of 1st pipe
			dup2(p1[0], 0); // 0 = STDIN_FILENO
			// replace grep's stdout with the write part of 2nd pipe
			dup2(p2[1], 1); // 1 = STDOUT_FILENO;
			// close all pipes
			close(p1[0]);
			close(p1[1]);
			close(p2[0]);
			close(p2[1]);
			execvp(*grep_args, grep_args); // execute grep
			exit(0);
		}
		else{
			// this is the parent process
			// replace wc's stdin with the write part of 2nd pipe
			dup2(p2[0], 0);
			// close all pipes
			close(p1[0]);
			close(p1[1]);
			close(p2[0]);
			close(p2[1]);
			execvp(*wc_args, wc_args); // execute wc

		} 

	}

	return 0;
}
