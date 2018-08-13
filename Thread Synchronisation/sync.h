/*  sync.h
 *
 *  Ankit Gola:                 2017EET2296
 *  Pushpendra Singh Dahiya:    2017EET2840
 *
 *  This file contains the struct and function declarations of the 
 *  functions defined in sync.c along with their brief description
 *
 */

// Define values
#define MAXMAXS 1024
#define RESET "\x1b[0m"
#define RED "\x1b[31m" 
#define GREEN "\x1b[32m" 
#define YELLOW "\x1b[33m" 
#define BLUE "\x1b[34m" 
#define MAGENTA "\x1b[35m" 
#define CYAN "\x1b[36m" 
#define BOLD "\x1b[1m"
#define ITALIC "\x1b[2m"

/*  Struct
 *
 *  Defines the struct type for the user
 *  uid                 :       User ID
 *  arrivalTime         :       Time when user arrives
 *  useTime             :       How long the user intends to keep serviceman busy (in seconds)
 *  state               :       Tells whteher a User is runnable, Can have either of following three values
 *                                  0b00    :   terminated
 *                                  0b01    :   running
 *                                  0b10    :   ready
 *  serviceMan          :       Later holds the number of serviceman servicing the process, default -1
 *  mutex               :       So than only one thread can access this process
 *  next                :       Pointer to next user
 *
 */
typedef struct USER {
    int uid;
    int arrivalTime;
    int useTime;
    int state; 
    pthread_mutex_t lock;
    int serviceMan;
    struct USER *next;
}USER;

// Global list
extern USER *list;
extern int maxProcess;
extern int maxServicemen;
extern int maxmaxs;

// Display a linked list
void display(USER *list);

// Create USER
USER *create_user(int uid);

// Sorted insert a uid as per arrival time
void sorted_insert(USER **head, int uid);

// Sorted insert a USER as per serviceman number
void sorted_insert2(USER **head, USER *user);

// Remove a USER from arbitrary position in list
USER *random_remove(USER **head, int uid);

// This function loops fro given amount of seconds
void loop_time(time_t sec);

// Print a list of instructions
void help_instructions();

// Handle command line input
void *handle_cmd();

// Get a count of USERS in list
int get_count(USER *list);

// Process USERs
void *processUser(void *serviceManID);

// Run a USER task
void run_task(USER *user);

// Handle servicemen
void *servicemen();

// Initialises the queue as per cmd
void init(int size);
