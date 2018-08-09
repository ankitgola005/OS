/*  shell.h
 *
 *  Ankit Gola:                 2017EET2296
 *  23.01.2018
 *
 *  This is the header file for "shell_main.c"
 *  It contains the function declarations of the functions called in main()
 *  This file contains only brief description of functions used. 
 *  For detailed function descriptions, check "shell.c"
 *
 */

// Shell loop
int sh_loop();

// Function to read user input from terminal
char *sh_readInput();

// Function to parse user input
char **sh_parseLine(char *line);

// Function to spawn a child process using fork() nad execvp()
int sh_procCreate(char **args);

// Function to implement "cd"
int sh_cd(char **args);

// Function to implement "exit"
int sh_exit(char **args);

// Function to count number of builtin functions
int sh_builtinCount();

// Function to decide whether input cammand is builtin or executable from PATH
int sh_call(char **args);

// Function to initialise dirs with cwd
int sh_setDirs(char *str);

// Function to switch directory and push cwd to stack
int sh_pushd(char **args);

// Function to pop from stack and switch directory.
int sh_popd(char **args);

// Function to display dirs stack
int sh_dirs(char **args);

// Function to modify PATH
int sh_path(char **args);

// Function to set enviornment(PATH variable)
int set_env();

// signal handler to capture ctrl+c
void INThandler(int sig);
