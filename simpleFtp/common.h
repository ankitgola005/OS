/*  common.h
 *
 *  Ankit Gola:                 2017EET2296
 *
 *  This is the common.h file.
 *  This file contains function declarations along with their
 *  brief descriptions. For detailed descriptions of these functions, 
 *  refer to common.c
 *
 */

// Function to read input from stdin
char *read_input();

// Function to parse a string based on DELIM
char **parse_line(char *line);

// Function to change directory
int f_cd(char **args, int clientSocket);

int f_close(char **args, int clientSocket);

int f_exit(char **args, int clientSocket);

int f_help(char **args, int clientSocket);

// Function to serialize a string of arrays
char *serialize(char **args);

int recv_file(int socket, char *file, int loggedIn, int client);

int send_file(int socket, char *text, int loggedIn, int client);

int call_client(char **args, int clientSocket, int loggedIn);

int execute_on_server(int clientSocket, char **args);

int proc_create(char **args);

int builtin_client_count();

int client_count();

int builtin_server_count();

int server_count();

int call_server(char **args, int clientSocket);

int proc_create_server(char *args);

int f_put_client(int clientSocket, char **args, int loggedIn);

int f_get_client(int clientSocket, char **args, int loggedIn);
