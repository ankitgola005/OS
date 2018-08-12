/*  sim.h
 *
 *  Ankit Gola:                 2017EET2296
 *
 *  This file contains structure and function declarations and their brief descriptions.
 *
 */

/*  Structure for the process
 *  procID          :   Process ID. Generated sequentially
 *  procType        :   Process type. CPU bound or IO bound
 *                          System CPU  :   11
 *                          System IO   :   10
 *                          User CPU    :   01
 *                          User IO     :   00
 *  state           :   Process can be in either of the following states. Process is created by default in waiting state
 *                          running     :   11
 *                          ready       :   10
 *                          waiting     :   01
 *                          terminated  :   00
 *  arrivalTime     :   Time at which process arrives in ready queue. Non decreasing numbers with respect to procID
 *  burstTime       :   CPU time required by the process
 *  burstTimeIO     :   Time after which process waits for IO, if it is IO bound
 *  priority        :   0 to 139, 0-99 for real time, 100-139 for users
 */            
typedef struct proc {
    int procID;
    int procType;
    int state;
    int arrivalTime;
    int priority;
    int burstTime;
    int burstTimeIO;
    struct proc *next;
} proc;

// Creates processs
proc* create_proc(int procId, int procType);

// Add a process at head
void insert_head(proc **list, proc *node);

// Generates random process type
int rand_gen(int userIO, int userCPU, int systemIO, int systemCPU);

// Find ceil
int find_ceil(int arr[], int r, int l, int h);

// Display an array
void disp(int *arr, int size);

// Display a linked list
void disp_list(proc *head);

// Sort Processes based on arrival time
void merge_sort(proc **head);

// Split the array at mid of list for merge sort algorithm
void front_back_split(proc *source, proc **frontRef, proc **backref);

// Merge two lists in a sorted manner
proc *sorted_merge(proc *a, proc *b);

// Initialise an array to 0
int initmem(int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int *throughput, int *loadAverage);

// FCFS original
int fcfs(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// SJF original
int sjf(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// SRTF
int srtf(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// Priority 1
int priority1(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// Priority pre emptive
int priority2(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// Round Robin
int round_robin(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage, int quanta);

// Multilevel Queue
int m_level(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// Multilevel Feedback Queue
int mlf_queue(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage);

// Average
float mean(int *arr, int size);

// Copy linked list
proc *copy(proc *head);

// get number of ready processes
int ready_proc(proc *head, int currentTime);

// Reset state
int set_state(proc *head);

// Return smaller of two numbers
int min(int num1, int num2);

// Pop a struct from list
proc *delete_tail(proc *list);

// Age a waiting process
int ageing(proc *list, proc *running, int flag);

// DElete a given node from linked list
proc *delete_i(proc **head, proc *process);

// Inserted sort in a linked list
void sorted_insert(proc **head, proc *newNode);

// Clear an array
void del(int *arr, int size);

// RB struct
typedef struct rb {
    proc *process;
    int color;
    struct rb *link[2];
} rb;

// Create new node
rb* createRBNode(proc *process);

// Insert in rb tree
void rbInsert(proc *process);

// Delete node
proc *rbDelete(rb **head);

