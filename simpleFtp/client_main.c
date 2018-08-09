/*  client_main.c
 *
 *  Ankit Gola:                 2017EET2296
 *  Pushpendra Singh Dahiya:    2017EET2680
 *
 *  This is the main file of ftp client.
 *  This creates a socket, connects to server and loops 
 *  for user commands. Commands are parsed and are either 
 *  serviced locally or on the server. Server response is then
 *  by the client.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "client.h"

int main (int argc, char *argv[]) {
    if(argc < 3) {
        fprintf(stderr, "USAGE: %s <server_name> <port>\n", argv[0]);
        return 1;
    }

    int clientSocket;

    // Establish link
    if(client_service_create(&clientSocket, argv[1], strtol(argv[2], (char**)NULL, 10))) {
        fprintf(stderr, "client_Service_create: failed to establish link\n");
        return 1;
    }
    // Establish session
    
    session(clientSocket);
    /*int loggedIn = 0;
    do {
        loggedIn = client_login(clientSocket, 2);
    } while(!loggedIn);

    // User command loop
    client_loop(clientSocket, loggedIn);
*/
    // Test code
    /*
    char buffer[64];
    bzero(buffer, 64);
    if(read(clientSocket, buffer, strlen(buffer)) < 0) {
        fprintf(stderr, "Cannot read from socket\n");
    }
    fprintf(stdout, "ftp> %s: ", buffer);

    bzero(buffer, 64);
    fgets(buffer, 64, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    
    if(write(clientSocket, buffer, 64) < 0) {
        fprintf(stderr, "Cannot write to socket\n");
    }
    fprintf(stdout, "To server: %s\n", buffer);
    */
    
    // Destroy session
    // Destroy link
    close(clientSocket);
    return 0;
}
