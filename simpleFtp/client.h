int client_service_create(int *clientSocket, char* serverName, int serverPort);

int client_login(int socket, int loggedIn);

int client_loop(int clientSocket, int loggedIn);

int session(int clientSocket);
