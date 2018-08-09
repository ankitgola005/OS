static sigjmp_buf env;

static volatile sig_atomic_t jumpActive = 0;

int server_service_create(int *socket, int port);

void INThandler(int signo);

int server_login(int socket, int numUser, char *userInfo[5][2]);

int server_loop(int clientSocket, int loggedIn);

int f_put_server(int clientSocket, char **args, int loggedIn);

int f_get_server(int clientSocket, char **args, int loggedIn);

int session_server(int clientSocket, int numUser, char *userInfo[5][2]);
