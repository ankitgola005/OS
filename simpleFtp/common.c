/*  common.c
 *
 *  Ankit Gola:                 2017EET2296
 *  Pushpendra Singh Dahiya:    2017EET2680
 *
 *  This is the common.c file.
 *  This file contains the functions common to both server and client.
 *  This file contains detailed description fo the functions called, including
 *  the number of arguments, type of arguments, and return type.
 *  For brief description of these functions, refer to common.h
 *
 */

// Header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"

// Defines
#define BUFSIZE 8
#define DELIM " \t\n\r\a"
#define MAXDATASIZE 8
#define MAX_BUFSIZE 8

/*  read_input()
 *
 *  This function takes no arguments.
 *  This function returns a string upon successful execution.
 *
 *  This function reads the input from stdin.
 */
char *read_input() {
    char * inputArgs = NULL;
    size_t bufsize = 0;
    int nr = getline(&inputArgs, &bufsize, stdin);
    
    if(nr < 0) {
        strcpy(inputArgs, "ctrld");
    }
    return inputArgs;
}

/*  parse_line()
 *
 *  This function takes a single argument.
 *  Argument is of type string.
 *  This fucntion returns an array of strings upon successful execution.
 *
 *  This function takes a string, and parses it 
 *  based upon the values defined in DELIM.
 */
char **parse_line(char *line) {
    int bufSize = BUFSIZE, index = 0;

    // Allocate memory for arguments
    char **argArr = (char **)malloc(sizeof(char *) * bufSize);
    // Exit if malloc fails to allocate memory
    if(!argArr) {
        fprintf(stderr, "parse_line: malloc error\n");
        exit(EXIT_FAILURE);
    }

    char *arg;
    // Parse string on basis of delimiters
    arg = strtok(line, DELIM);
    while(arg != NULL) {
        argArr[index] = arg;            // Save args in array
        index++;

        // If index exceeds pre-allocated buffer size,
        // reallocate a bigger buffer
        if(index >= bufSize) {
            bufSize += BUFSIZE;
            argArr = realloc(argArr, sizeof(char*) * bufSize);

            if(!argArr) {
                fprintf(stderr, "parse_line: relloc error\n");
                exit(EXIT_FAILURE);
            }
        }
        
        arg = strtok(NULL, DELIM);
    }

    // Set last element of array as NULL
    argArr[index] = NULL;

    return argArr;
}



// Built in functions of clients
char *builtinClientArr[] = {
    "help",
    "lcd",
    "close",
    "exit",
};

// Valid functions in client
char *clientArr[] = {
    "lls",
    "lchmod",
    "get",
    "put",
    "lcd",
    "close",
};

// Built in functions of server
char *builtinServerArr[] = {
    "cd",
};

// Valid functions in server
char *serverArr[] = {
    "ls",
    "chmod",
    "cd",
    "put",
    "get",
};

/*  Function Pointers   */
// For builtin functions of server
int (*builtinServer[]) (char **args, int clientSocket) = {
    &f_cd,
};

// For builtin functions of client
int (*builtinClient[]) (char **args, int clientSocket) = {
    &f_help,
    &f_cd,
    &f_close,
    &f_exit,
};

/*  These functions return number of elements in arrays */
// For builtin functions of client
int builtin_client_count() {
    return (sizeof(builtinClientArr)/sizeof(char *));
}

// For total valid functions on client
int client_count() {
    return (sizeof(clientArr)/sizeof(char *));
}

// For builtin functions of server
int builtin_server_count() {
    return (sizeof(builtinServerArr)/sizeof(char *));
}

// For total valid functions on server
int server_count() {
    return (sizeof(serverArr)/sizeof(char *));
}

/*  f_cd() 
 *
 *  This function takes two arguments.
 *  Arguments are of type, array of strings, and int.
 *  This function returns 1 upon successful execution.
 *  
 *  This function changes the cwd of the shell,
 *  Result is observable in client, irrespective of whether it was 
 *  called in client or the server.
 */
int f_cd(char **args, int clientSocket) {
    // If no path is specified, cd to HOME directory
    if(args[1] == NULL || !strcmp(args[1], "~")) {
        if(chdir(getenv("HOME"))) {
            fprintf(stderr, "f_cd: home cd failure\n");
        }
        return 0;
    } // If path is specified
    else {
        if(chdir(args[1])) { 
            fprintf(stderr, "f_cd: cd failure\n");
        }
    }
    return 1;
}

/*  f_close()
 *
 *  This funciton takes two arguments.
 *  Arguments are of type, array of strings, and int.
 *  This function returns 0 upon successful execution.
 *
 *  This function logs out of ftp server.
 */
int f_close(char **args, int clientSocket) {
    // Send logout status to server
    char buffer[7] = "logout";
    if(write(clientSocket, buffer, sizeof(buffer)) < 0) {
        fprintf(stderr, "f_close: error logging out\n");
        return 1;
    }
    printf("Goodbye\n");
    return -1;
}

/*  f_exit()
 *
 *  This funciton takes two arguments.
 *  Arguments are of type, array of strings, and int.
 *  This function returns 0 upon successful execution.
 *
 *  This function logs out of ftp server, and terminates the ftp connection.
 */
int f_exit(char **args, int clientSocket) {
    // Send exit status to server
    char buffer[5] = "exit";
    if(write(clientSocket, buffer, sizeof(buffer)) < 0) {
        fprintf(stderr, "f_close: error logging out\n");
        return 1;
    }
    printf("Goodbye\nExiting ftp\n");
    return 0;
}

/*  f_help()
 *
 *  This function takes two arguments.
 *  Arguments are of type, array of strings, and int.
 *  This function returns 1 upon successful execution.
 *
 *  This functions displays help menu
 */
int f_help(char **args, int clientSocket) {
    printf("\n");
    printf("ls\t\t\t\tDisplays contents of server cwd\n");
    printf("lls\t\t\t\tDisplays contents of client cwd\n");
    printf("cd <path>\t\t\tDChanges path of server cwd\n");
    printf("lcd <path>\t\t\tChanges path of client cwd\n");
    printf("lchmod <filename> <attribute>\tChanges permissions of server file\n");
    printf("chmod <filename> <attribute>\tChanges permissions of client file\n");
    printf("get <filename>\t\t\tDownload server <filename> to client cwd\n");
    printf("put <filename>\t\t\tUpload client <filename> to server cwd\n");
    printf("help\t\t\t\tDisplay this message\n");
    printf("close\t\t\t\tLogs out of session\n");
    printf("exit\t\t\t\tTerminates the session, equivalent to \"bye\", and \"ctrl+d\" \n");

    return 1;
}

/*  f_put_client()
 *
 *  This function takes three arguments
 *  Arguments are of type, int, array of stringsm and int.
 *  Thsi functions returns 1 upon successful execution.
 *
 *  This function, checks user rights and if valid, writes a file on server.
 */
int f_put_client(int clientSocket, char **args, int loggedIn) {
    if(loggedIn == 1) {
        send_file(clientSocket, args[1], loggedIn, 1);
    } else {
        printf("You do not have enough privilages to write on server\n");
    }
    return 1;
}

/*  f_get_client()
 *
 *  This function takes three arguments
 *  Arguments are of type, int, array of stringsm and int.
 *  Thsi functions returns 1 upon successful execution.
 *
 *  This function gets a file from server.
 */
int f_get_client(int clientSocket, char **args, int loggedIn) {
    recv_file(clientSocket, args[1], loggedIn, 1);
    return 1;
}

/*  send_file()
 *
 *  This function takes four arguments.
 *  Arcuments are of type int, array of strings, int, and int.
 *  This function returns 1 upon successful execution.
 *
 *  This function is used to send file over a socket.
 *  It first checks for existence of file, and if it exists, sends it over socket.
 */
int send_file(int socket, char *file, int loggedIn, int client) {
    FILE *fp;
    char ch, buf[512];
   // printf("%s\n", file);
    int error = 0;

    char temp[32];
    // Check if file exists,
    if(!access(file, F_OK | R_OK)) {
        strcpy(temp, "ok");
//        printf("1exist %s, %d\n", temp, error);
    } else {
        strcpy(temp, "error");
        error = 1;
  //      printf("2error %s, %d\n", temp, error);
    }
   
    // Send status to reciever
    write(socket, temp, sizeof(temp));
  //  printf("3 %s, %d\n", temp, error);
    bzero(temp, 32);
    
    // Recieve status from reciever
    read(socket, temp, sizeof(temp));
    if(!strcmp(temp, "error")) {
        error = 1;
    }
    //printf("4response %s, %d\n", temp, error);

    // If error, return
    if(error) {
        return 1;
    }

    // Open file
    if((fp = fopen(file, "rb")) == NULL) {
        fprintf(stderr, "send_file: Cannot open file\n");
        return 1;
    }

    // Get file size
    struct stat stat_buf;
    stat(file, &stat_buf);
    int tempSize = stat_buf.st_size;
    int size = tempSize;
    //printf("%d\n", tempSize);
    
    int i = 0;
    bzero(buf, 512);
    //printf("before : %s\n", buf);
    for(i = 0; tempSize != 0; i++, tempSize = tempSize/10) {
        buf[i] = 48 + tempSize%10;
      //  printf("during: %d %s\n", tempSize%10, buf);
    }
    buf[i] = '\0';
    //printf("after: %s\n", buf);

    // Send Filesize
    write(socket, buf, sizeof(buf));

    // Send file while sent data < filesize
    i = 0;
    int sent = 0, numBytes;
    while(sent <= size) {
        ch = getc(fp);
        if(ch == EOF) {
    //        printf("%s\n", buf);
            if(i > 0) {
                if((numBytes = send(socket, buf, i, 0)) == -1) {
                    fprintf(stderr, "send_file: Error sending file\n");
                    return 1;
                }
            }
            sent += numBytes;
            break;
        }

        buf[i++] = ch;

        if(i == 512) {
   //         printf("%s\n", buf);
            if((numBytes = send(socket, buf, i, 0)) == -1) {
                fprintf(stderr, "send_file: Error sending file\n");
                return 1;
            }
            i = 0;
            sent += numBytes;
        }
    }
    printf("send: %d\n", sent);
    
    fclose(fp);
    return 1;
}

/*  recv_file()
 *
 *  This function takes four arguments.
 *  Arcuments are of type int, array of strings, int, and int.
 *  This function returns 1 upon successful execution.
 *
 *  This function is used to recieve file over a socket.
 *  It first checks for permissions, and if it valid, recieve a file over socket.
 */
int recv_file(int socket, char *file, int loggedIn, int client) {
    FILE *fp;
    char buf[512];
    //printf("%s\n", file);
    //printf("here recv\n");
    
    int error = 0;
    char temp[32];

    // Recieve file status
    printf("recvd : %ld\n", read(socket, temp, sizeof(temp)));
    if(!strcmp(temp, "error")) {
        error = 1;
      //  printf("1 from sender %s %d\n", temp, error);
    }

    // Check if directory is write protected
    if(!error) {
        char tempDir[PATH_MAX];
        getcwd(tempDir, PATH_MAX);
 //       printf("2. cwd %s, %d\n", tempDir, error);
        if(access(tempDir, F_OK | W_OK) != 0) {
            fprintf(stderr, "recv_file: Dest folder is write protected\n");
            error = 1;
   //         printf("3. cwd error, %d\n", error);
        }
        char overwrite;
        // Check if file exists already and is writable
        if(!access(file, F_OK)) {
            if(access(file, W_OK)) {
                fprintf(stdout, "File already exists and is write protected\n");
                error = 1;
           //     printf("4 file error, %d\n", error);
            } else {
                if(!error) {
                    if(client == 0 && loggedIn == 1) {
                        fprintf(stdout, "File already exists. Overwriting\n");
                        error = 0;
                    } else if (client == 0 && loggedIn == 2) {
                        fprintf(stdout, "Yo do not have enough privilages on server\n");
                        error = 1;
                    } else if (client == 1) {
                        fprintf(stdout, "File already exists. Overwrite?[y/n]\n");
                        scanf("%c", &overwrite);
                        if(overwrite != 'y') {
                           error = 1;
                        } else {
                            error = 0;
                        }
                    }
                }
            }
        }
    }


    // Send error status to server
    bzero(temp, 32);
    if(error) {
        strcpy(temp, "error");
        write(socket, temp, sizeof(temp));
    } else {
        strcpy(temp, "ok");
        write(socket, temp, 32);
    }

    if(error) {
        return 1;
    }

    //remove(file);

    // Create file on reciever
    if((fp = fopen(file, "wb")) == NULL) {
        fprintf(stderr, "recv_file: Cannot open file on reciever\n");
        return 1;
    }
    
    // Recieve Filesize
    if(read(socket, buf, 512) == -1) {
        fprintf(stderr, "recv_file: Cannot recieve size of file\n");
        return 1;
    }
//    printf("recieved: %s\n", buf);

    int size = 0;
    for(int i = 0, mul = 1; buf[i] != '\0'; i++, mul *= 10) {
        size += (buf[i] - 48) * mul;
        //printf("during: %d, %c\n", size, buf[i]-48);
    }
    printf("%d\n", size);

    bzero(buf,512);
    // Recieve file while recieved data is less than filesize
    int recieved = 0, numBytes = 0;
    while(recieved < size) {
        if((numBytes = recv(socket, buf, 512, 0)) == -1) {
            fprintf(stderr, "recv_file: Error reading from other side\n");
            return 1;
        }
        
  //      printf("%d, %d: %s\n", recieved, numBytes, buf); 
        fwrite(buf, sizeof(buf[0]), numBytes, fp);
        recieved += numBytes;
    }

    printf("Recieved: %d\n", recieved);   
    fclose(fp);

    return 1;
}


/*  call_client
 *
 *  This function takes two arguments.
 *  Arguments are of type, array of string, and int.
 *  This funnction executes the commands.
 *  First is looks if the command is builtin, if yes, execute it.
 *  If not, it calls another function to fork a new process to execute it.
 */
int call_client(char **args, int clientSocket, int loggedIn) {
    printf("starting call_client with %s\n", args[0]);

    if(!args[0]) {
        return 1;           // If no function name passed, return 1
    }
   
    int validCommand = 0;

    // If puts or gets is executed, send corresponding command to server to get ready
    if(!strcmp(args[0], "put")) {
        // Send command to server
    //    printf("client got: %s\n", args[0]);
        strcpy(args[0], "get");
        execute_on_server(clientSocket, args);
    //    printf("out ot exec on server\n");
        
        f_put_client(clientSocket, args, loggedIn);
        validCommand = 1;
        return 1;
    }
    if(!strcmp(args[0], "get")){
        // Send command to server
      //  printf("client got: %s\n", args[0]);
        strcpy(args[0], "put");
        execute_on_server(clientSocket, args);
     //   printf("out ot exec on client\n");
        
        f_get_client(clientSocket, args, loggedIn);
        validCommand = 1;
        return 1;
    }

    // Search for called function in list of builtin functions
    // and call the particular function
    for(int i = 0; i < builtin_client_count(); i++) {
        if(!strcmp(args[0], builtinClientArr[i])) {
            validCommand = 1;
            return (*builtinClient[i])(args, clientSocket);
        }
    }
    
    // If function is to be executed on the client
    for(int i = 0; i < client_count(); i++) {
    //    printf("%s may match with %s\n", args[0], clientArr[i]);
        if(!strcmp(args[0], clientArr[i])) {
            validCommand = 1;
            for(int j = 1; j <= strlen(args[0]); j++){
                args[0][j-1] = args[0][j];
            }
    //        printf("%s\n", args[0]);
            return proc_create(args);
        }
    }

    // If function is not builtin or client, send to server
    // If function is to be executed over server
    for(int i = 0; i < server_count(); i++) {
        if(!strcmp(args[0], serverArr[i])) {
            validCommand = 1;
            if(!strcmp(args[0], "chmod") && loggedIn == 2) {
                printf("You do not have enough privilages on the server\nContact your administrator if you think this is an error\n");
                return 1;
            }
            return execute_on_server(clientSocket, args);
        }
    }

    // If command is not recognised by the ftp, print help
    if(!validCommand){
        printf("Not a valid command\n");
        // Print help function
        return (*builtinClient[0])(args, clientSocket);
    }
    return 1;
}


/*  serialize()
 *
 *  This function takes in single argument.
 *  Argument is of type array of strings.
 *  This function returns a string upon successful execution.
 *
 *  This function takes an array of strings and converts it into
 *  a single string of space seperated words.
 */
char *serialize(char **args){
    char *serverString = NULL, temp[PATH_MAX];
    // Copy first string
    strcpy(temp, args[0]);
    // Concatenate args till end of array
    for(int i = 1; args[i] != NULL; i++) {
        strcat(temp, " ");
        strcat(temp, args[i]);
    }
    serverString = temp;
    
    return serverString;
}


/*  proc_create()
 *
 *  This function creates a single argument
 *  Argument is an array of string.
 *  This functions returns 1 upon successful execution.
 *  
 *  This function forks a new process to execute a command.
 */
int proc_create(char **args) {
    pid_t pid;

    if((pid = fork()) == 0) {
        // Child process execv()
        signal(SIGINT, SIG_DFL);
        if(execvp(args[0], args) == -1) {
            fprintf(stderr, "proc_create: failed to launch execvp()\n");
        }
        return 0;
    } else if(pid > 0){
        // Parent process wait till child terminates
        waitpid(-1, NULL, 0);
    }
    return 1;
}

/*  execute_on_server()
 *
 *  This function takes two arguments.
 *  Arguments are of type int, and an array of strings.
 *  This function returns 1 upon successful execution.
 *
 *  This function sends a query to server for execution,
 *  and then reads the server response and displays it on stdin.
 */
int execute_on_server(int clientSocket, char **args) {
    // Serialize
    printf("in execute_on_Server: %s\n", args[0]);
    char buffer[PATH_MAX];
    strcpy(buffer, serialize(args));
   
    printf("Buffer:%s\n", buffer);
    // Send login status
    
    // Send to server
    if(write(clientSocket, buffer, 32) < 0) {
        fprintf(stderr, "execute_on_server: write error\n");
        return 1;
    } 
   
    bzero(buffer, sizeof(buffer));
    // Read response from server
    if(read(clientSocket, buffer, sizeof(buffer)) < 0) {
        fprintf(stderr, "execute_on_server: write error\n");
        return 1;
    }
    // Display Response
    fprintf(stdout, "%s\n", buffer);
    return 1;
}

/*  call_server()
 *
 *  This function takes two arguments
 *  Arguments are of type, array of string, and an int.
 *  This function returns 1 upon successful execution.
 *
 *  This function executes the commands on the server.
 *  It first looks in list of builtin functions, if it exists, execute it.
 *  If not, it calls another process to fork and execute the command.
 *  Then it redirects result of the fork to a file(if any), loads the file into a 
 *  buffer and send the buffer back to client.
 */
int call_server(char **args, int clientSocket) {
    // If no args exist
    if(!args[0]) {
        return 1;
    }
    
    // Check built in functions for command
    for(int i = 0; i < builtin_server_count(); i++) {
        //printf("%s may match %s\n", args[0], builtinServerArr[i]);
        if(!strcmp(args[0], builtinServerArr[i])) {
            return (*builtinServer[i])(args, clientSocket);
        }
    }
    
    // Serialize the args and redirect output of command to a remote hidden file on server.
    char buf[PATH_MAX];
    strcpy(buf, serialize(args));
    strcat(buf, ">.file");
    return proc_create_server(buf);
}


/*  proc_create_server()
 *  
 *  This function takes in one argument.
 *  Argument is of type string.
 *  This function returns 1 upon successful execution.
 *
 *  This function forks a new process, and then calls execlp to execute the command.
 */
int proc_create_server(char *args) {
    pid_t pid = vfork();
    char *buffer[PATH_MAX];
    bzero(buffer, PATH_MAX);
   
    if(pid == 0) {
        execlp("sh", "sh", "-c", args, (char*)0);
    } else {
        waitpid(-1, NULL, 0);
    }
    return 1;
}

