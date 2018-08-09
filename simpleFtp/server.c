/*  server.c
 *
 *  Ankit Gola:                 2017EET2296
 *  Pushpendra Singh Dahiya:    2017EET2680
 *
 *  This is the function file for server_main.c
 *  This file contains the function definations called in the program.
 *  Detailed description of functions, along with number of arguments,
 *  argument type, and return type are mentioned at appropriate places.
 *  For brief description, refer to server.h.
 *
 */

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <setjmp.h>
#include <limits.h>
#include <sys/wait.h>

#include "server.h"
#include "common.h"

// Defines
#define SERVER_SOCKET_BACKLOG 128

/*  INThandler
 *  
 *  This function takes a single argument.
 *  Argument is of type int.
 *  This function returns nothing.
 *
 *  This function is a custom signal handler to block ctrl+c/SIGINT
 */
void INThandler(int signo) {
    if(!jumpActive) {
        return;
    }
    longjmp(env, 42);
}

/*  server_service_create()
 *
 *  This function takes two arguments.
 *  Arguments are of type, int* and int.
 *  This function returns 0 upon successful execution.
 *
 *  This function creates the basic socket routine.
 *  It creates a socket, sets options for it, 
 *  forcefully binds the socket to a port, and listens to socket for client connections.
 */
int server_service_create(int *serverSocket, int port) {
    struct sockaddr_in serverAddress;

    // Initialise serverAddress struct
    memset(&serverAddress, 0, sizeof(serverAddress));
    
    // Set struct elements for serverAddress
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    // Create Socket
    if((*serverSocket = socket(serverAddress.sin_family, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "server_service_create: Cannot create socket\n");
        return 1;
    }
    
    // Set options for address and port reusability
    int option = 1;
    if(setsockopt(*serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(int)) < 0) {
        fprintf(stderr, "server_service_create: Cannot set options\n");
        return 1;
    }

    // Bind socket to port
    if(bind(*serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "server_service_create: Cannot bind socket\n");
        close(*serverSocket);
        return 1;
    }

    // Listen to socket for clients
    if(listen(*serverSocket, SERVER_SOCKET_BACKLOG) < 0) {
        fprintf(stderr, "server_service_create: Cannot listen on socket\n");
        close(*serverSocket);
        return 1;
    }

    return 0;
}

/*  server_login()
 *
 *  This function takes three arguments.
 *  Arguments are of type int, int, and a 2D string array.
 *  This functions returns an int upon successful execution.
 *
 *  This function is supplied database of username and passwoords.
 *  Then it queries with client for username and password.
 *  If client provides correct combination of the two, it creates a session, depending on access level of user.
 *  Return value can be :
 *      1 : Anonymous       Only read rights.
 *      2 : Admin           Full rights.(cannot modify system directories for security reasons).
 */
int server_login(int socket, int numUser, char *userInfo[5][2]) {
    char buffer[64];
    char username[64], password[64];
    bzero(buffer, 64);
    
    // Recieve logged in status and continue if loggedin = 0
    if(read(socket, buffer, 64) > 0) {
        if(!strcmp(buffer, "1") || !strcmp(buffer, "2")) {
            printf("User logged in: %d\n", atoi(buffer));
            return atoi(buffer);
        }
    }
    
    // Read username
    if(read(socket, buffer, 64) > 0) {
        strcpy(username, buffer);
    }
    printf("uname: %s\n", buffer);
    bzero(buffer, 64);
    // Test: fprintf(stdout, "%s:\n", username);
    
    // If username is anonymous, don't require a password to login
    if(!strcmp(username, "anonymous")) {
        strcpy(buffer, "Login successful. Restricted account");
        if(write(socket, buffer, 64) < 0) {
            fprintf(stderr, "Login reqponse failure\n");
        }
        return 2;               // Return value 2 indicates user is anonymous
    }

    // Ask client for password
    strcpy(buffer, "password");
    if(write(socket, buffer, 64) < 0) {
        fprintf(stderr, "Password request failure\n");
    }

    // Read password from client
    if(read(socket, buffer, 64) > 0) {
        strcpy(password, buffer);
    }
    printf("pass: %s\n", buffer);
    
    // If user is admin and password matches
 /*   if(!strcmp(username, "admin") && !strcmp(password, "admin")) {
        strcpy(buffer, "Login Successful.");
        if(write(socket, buffer, 64) < 0) {
            fprintf(stderr, "Login response failure\n");
        }
        return 1;           // Return value 1 indicates user is admin
    }
*/
    // Check if user is Admin
    bzero(buffer, 64);
    for(int i = 0; i < numUser; i++) {
        // Look for match in username
        if(!strcmp(username, userInfo[i][0])) {
            // Look fro match in password
            if(!strcmp(password, userInfo[i][1])) {
                // If match, send success response to client
                strcpy(buffer, "Login Successful.");
                // Send response to client
                write(socket, buffer, 64);
                return 1;       // Return value 1 indicates any registered user
            } else {
                // Incorrect password
                strcpy(buffer, "Incorrect username or password");
                // Send response to client
                write(socket, buffer, 64);
                return 0;
            }
        }

        // If username is not found
        if(i == numUser-1) {
            strcpy(buffer, "User does not exist");
            write(socket, buffer, 64);
            return 0;
        }
    }

    return 0;
}

/*  server_loop()
 *
 *  This function takes in two arguments.
 *  Arguments are of type int, and int.
 *  This function returns 0 upon successful execution.
 *
 *  This function loops and listens for client command, 
 *  execute it over the server(if required), and then send either the result of command, 
 *  if one exists, or send a Success message to the client.
 */
int server_loop(int clientSocket, int loggedIn) {
    char buffer[PATH_MAX];
    // While client is logged in
    printf("starting server_loop\n");
    while(loggedIn) {
        if(read(clientSocket, buffer, 32) < 0) {
            fprintf(stderr, "server_loop: cannot read from client\n");
            return 1;
        }
     
        printf("from client: %s\n", buffer);
        // Check if client logged out
        if(!strcmp(buffer, "logout")) {
            loggedIn = 0;
            return -1;
        }

        if(!strcmp(buffer, "exit")) {
            loggedIn = 0;
            return 0;
        }

        // Parse the command recieved from client 
        char **args;
        args = parse_line(buffer);
        int ls = 0, get = 0, put = 0;
        
        if(!strcmp(args[0], "ls")){
            strcat(buffer, " >.file");
            ls = 1;
        }

        if(!strcmp(args[0], "get")) {
	    //    printf("Server recieved: %s\n", args[1]);
            //strcpy(buffer, "Saving");
            if(write(clientSocket, "Saving", 32) < 0) {
                fprintf(stderr, "server_loop: cannot read from client\n");
                return 1;
            }
            //bzero(buffer, sizeof(buffer));
            printf("starting get_server\n");
	        f_get_server(clientSocket, args, loggedIn);   
            get = 1;

        }
	
        //printf("%s: %s\n", args[0], args[1]);
        if(!strcmp(args[0], "put")) {
	        //printf("Server recieved: %s, %s \n", args[0], args[1]);
            //strcpy(buffer, "Sending");
	        if(write(clientSocket, "Sending", 1024) < 0) {
                fprintf(stderr, "server_loop: cannot read from client\n");
                return 1;
            }
            //bzero(buffer, sizeof(buffer));
            //printf("%s: %s\n", args[0], args[1]);
            printf("starting put_server: %s\n", args[1]);
            f_put_server(clientSocket, args, loggedIn);
            put = 1;
        }


        // Call server, to execute the command
        if(!get && !put){
            call_server(args, clientSocket);
        }

        bzero(buffer, sizeof(buffer));
        strcpy(buffer, "Success from server\n");
        
        if(ls){
            FILE *fp;
            char *line = NULL;
            size_t len = 0;
            ssize_t read;
            bzero(buffer, sizeof(buffer));

            fp = fopen(".file", "r");
            if(fp == NULL){
                fprintf(stderr, "server_loop: cannot open .file\n");
                exit(EXIT_FAILURE);
            }

            read = getline(&line, &len, fp);
            strcpy(buffer, line);
            while((read = getline(&line, &len, fp)) != -1) {
                //line[strlen(line) - 1] = '\t';
                strcat(buffer, line);
            }
            fclose(fp);
            system("rm .file");
            if(line)
                free(line);
        }    
        
        // Send the buffer with required information back to the client
        if(!put && !get) { 
            if(write(clientSocket, buffer, 1024) < 0) {
                fprintf(stderr, "server_loop: cannot read from client\n");
                return 1;
            }
        }
    }
    return 0;
}

int f_get_server(int clientSocket, char **args, int loggedIn) {
    if(loggedIn == 1) {
        recv_file(clientSocket, args[1], loggedIn, 0);
    }
    return 1;
}

int f_put_server(int clientSocket, char **args, int loggedIn) {
    send_file(clientSocket, args[1], loggedIn, 0);
    return 1;
}

int session_server(int clientSocket, int numUser, char *userInfo[5][2]) {
    while(1) {
        int loggedIn = 0;
        do {
            loggedIn = server_login(clientSocket, numUser, userInfo);
        } while(!loggedIn);

        printf("login value : %d\n", loggedIn);
        
        int ret_val = server_loop(clientSocket, loggedIn);
       
        printf("ret_val %d\n", ret_val);
        if(ret_val == -1) {
            continue;
        }
        
        if(ret_val == 0); {
            return 0;
        }
    }
}
