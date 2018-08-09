/*  server_main.c
 *
 *  Ankit Gola:                 2017EET2296
 *  Pushpendra Singh Dahiya:    
 *
 *  This is the main file of ftp server.
 *  This creates a socket, connects to it, and listens for clients.
 *  More than one client may connect to the server.
 *  Once a client is connected, server forks a new process to execute 
 *  client commands and sends the result back to server.
 *
 */

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include "server.h"

/*  main()
 *  This function takes two arguments.
 *  Arguments are of type int, and an array of strings.
 *  This function returns 0 upon successful execution.
 *  
 *  This is the makin file for server.
 */
int main(int argc, char *argv[]) {
    int port;
    if(argc < 2) {
        // Default port number, bind to 21 (linux ftp port) may fail
        port = 8080;
    } else {
        // Else use port supplied by user
        port = atoi(argv[1]);
    }
    
    int serverSocket, clientSocket, childPID;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize;
    int numUser = 3;

    // DEfault users and passwords
    char *userInfo[5][2] = {
                                {"su", "su"},
                                {"ankit", "ankit"},
                                {"psd", "psd"},
                           };

    // TODO :Signal handling
    // signal(SIGINT, INThandler);

    // Create service
    if(server_service_create(&serverSocket, port)){
        fprintf(stderr, "server_service_create: Failed to create service\n");
        return 1;
    }

    // Loop for clients
    while(1) {
        if(sigsetjmp(env, 1) == 42) {
            printf("ftp> \n");
        }
        jumpActive = 1;

        // Wait for client. This is a blocking call
        clientAddressSize = sizeof(clientAddress);
        if((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressSize)) > 0) { 
            // Child Process
            if((childPID = fork()) == 0) {
                // Child doesn't need this socket
                close(serverSocket);
            
                /*
                do {
                    // loggedIn : 
                    //              1 :   Admin
                    //              2 :   Anonymous
                    loggedIn = server_login(clientSocket, numUser, userInfo);
                } while(!loggedIn);
                
                // Job loop 
                
                server_loop(clientSocket, loggedIn);
                // Finished with client
                */
                
                session_server(clientSocket, numUser, userInfo);

                close(clientSocket);
                return 0;
            }
        } else {
            fprintf(stderr, "accept: cannot accept client connection\n");
        }
    }
    close(serverSocket);

    return 0;
}
