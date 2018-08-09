/*  client.c
 *
 *  Ankit Gola:                 2017EET2296
 *  Pushpendra Singh Dahiya:    2017EET2680
 *
 *  This is the client.c file.
 *  It contains all the function definations called in client_main.c,
 *  along with detailed description of the functions, including 
 *  the number of arguments, type of arguments, and the return type of each function.
 *  For brief description of these functions, refer to client.h
 *
 */

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <limits.h>

#include "client.h"
#include "common.h"

/*  client_service_create()
 *
 *  This function takes in three arguments.
 *  Arguments are of type int*, string, and int.
 *  This function returns 1 upon successful execution.
 *
 *  This function takes a servername and server port and creates a socket to connect ot the server.
 *
 */
int client_service_create(int *clientSocket, char *serverName, int serverPort) {
    struct sockaddr_in serverAddress;
    struct hostent *serverInfo;

    // Create server address
    memset(&serverAddress, 0, sizeof(serverAddress));

    // Determine dotted quad from dns query
    if((serverInfo = gethostbyname(serverName)) == NULL) {
        fprintf(stderr, "client_service_create: cannot get dotted quad string\n");
        return 1;
    }

    // Initialise serverAddress struct elements
    serverAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)serverInfo->h_addr)));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);

    // Create socket
    if((*clientSocket = socket(serverAddress.sin_family, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "client_service_create: cannot create socket\n");
        return 1;
    }
    
    // Connect to server socket
    if((connect(*clientSocket, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr))) < 0) {
        fprintf(stderr, "client_service_create: cannot connect to socket\n");
        close(*clientSocket);
        return 1;
    }
    return 0;
}

/*  client_login()
 *
 *  This function takes in single argument.
 *  Argument is of type int.
 *  This function returns 1 upon successful execution.
 *
 *  This function logs in the user at client end.
 */
int client_login(int socket, int loggedIn) {
    char buffer[64];
    bzero(buffer, 64);

    if(loggedIn == 1) {
        strcpy(buffer, "1");
    } else if (loggedIn == 2){
        strcpy(buffer, "2");
    } else {
        strcpy(buffer, "out");
    }

    // Send login status
    if(write(socket, buffer, 64) < 0) {
        fprintf(stderr, "client_login: error sending username\n");
        return 0;
    }

    if(loggedIn) {
        return loggedIn;
    }

    // Get username
    fprintf(stdout, "ftp> Username: ");
    fgets(buffer, 64, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    
    // Send username to server
    if(write(socket, buffer, 64) < 0) {
        fprintf(stderr, "client_login: error sending username\n");
        return 0;
    }
    
    // Get username response
    bzero(buffer, 64);
    if(read(socket, buffer, 64) < 0) {
        fprintf(stderr, "client_login: error reading username response\n");
        return 0;
    } 
    
    // Request password
    else if(!strcmp(buffer, "password")) {
        fprintf(stdout, "ftp> Password: ");
        fgets(buffer, 64, stdin);
        buffer[strlen(buffer) - 1] = '\0';
        
        // Send password to server
        if(write(socket, buffer, 64) < 0) {
            fprintf(stderr, "client_login: error sending username\n");
            return 0;
        }
    }
    
    // Anonymous login
    else {
        fprintf(stdout, "%s\n", buffer);
        return 2;
    }
    
    // Password response 
    bzero(buffer, 64);
    if(read(socket, buffer, 64) < 0) {
        fprintf(stderr, "Login response read failure\n");
        return 0;
    }
    
    // If login is successful
    else if(!strcmp(buffer, "Login Successful.")) {
        fprintf(stdout, "%s\n", buffer);
        return 1;
    } else {
        fprintf(stdout, "%s\n", buffer);
        return 0;
    }
    return 0;
}

/*  client_loop()
 *
 *  This function takes in a single argument.
 *  Argument is of type int.
 *  This function returns 1 upon successful execution.
 *
 *  Thsi function loops to take in user input, 
 *  parse it based on DELIM, and calls call_client to execute the command.
 */
int client_loop(int clientSocket, int loggedIn) {
    // Read input
    printf("starting client_loop\n");
    char *input;                // Hold input string
    char **args;                // Hold arguments as array of strings
    int status = 1;

    do {
        printf("ftp> ");
        // Call a function to read user input from terminal till endline
        input = read_input();
        if(!strcmp(input, "\n")) {
            continue;
        }

        // If ctrl+D is presssed, Exit ftp client
        if(!strcmp(input, "ctrld")) {
            printf("\n");
            return 0;
        }

        // Call a function to parse user input into arguments
        args = parse_line(input);
        if(!strcmp(args[0], "bye")) {
            strcpy(args[0], "close");
        }
/*
        if(!strcmp(args[0], "put")) {
            f_put_client(clientSocket, args, serverAddress, serverPort);
            continue;
        }

        if(!strcmp(args[0], "get")) {
            f_get_client(clientSocket, args, serverAddress, serverPort);
            continue;
        }
  */      // Call call_client to execute the command appropriately
        status = call_client(args, clientSocket, loggedIn);
        printf("status: %d\n", status);
        if(status == -1){
            loggedIn = 0;
        }
        if(status == 0) {
            loggedIn = 0;
        }

        free(input);
        free(args);
    } while(loggedIn); 
   
    return status;
}

int session(int clientSocket) {
    while(1) {
        int loggedIn = 0;
        do {
            loggedIn = client_login(clientSocket, loggedIn);
        } while(!loggedIn);

        printf("on client login: %d\n", loggedIn);
        int sessionEnable = client_loop(clientSocket, loggedIn);

        if(sessionEnable == -1){
            continue;
        }
        if(sessionEnable == 0) {
            break;
        }
    }
    return 0;
}
