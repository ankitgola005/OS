/*  sim.c
 *  
 *  Ankit Gola              :   2017EET2296
 *
 *  This file contains function definations of all the functions called in main,
 *  along with their description.
 *
 */

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#include "sim.h"

/*  disp()
 *  
 *  This function takes two arguments
 *  or type array and int
 *  This function returns nothing
 *
 *  This funtion displays the contents of a 1-D array
 */
void disp(int *arr, int size) {
    for(int i = 0; i < size; i++) {
        printf("%d\t", arr[i]);
    }
    printf("\n");
}

/*  disp_list()
 *
 *  This function takes one argument
 *  of type struct proc
 *  This function returns nothing
 *  
 *  This function displays the contents of a linked list of type struct *proc
 */
void disp_list(proc *head) {
    proc *p;
    // If head is null
    if(head == NULL)
        return;

    // Iterate through the list and display it's contents
    for(p = head; p != NULL; p = p->next) {
        printf("procID      : %d\n", p->procID);
        printf("procType    : %d\n", p->procType);
        printf("state       : %d\n", p->state);
        printf("arrivalTime : %d\n", p->arrivalTime);
        printf("burstTime   : %d\n", p->burstTime);
        printf("burstTimeIO : %d\n", p->burstTimeIO);
        printf("priority    : %d\n", p->priority);
        printf("\n");
    }

    // End of list
    printf("NULL\n");
}

/*  find_ceil
 *
 *  This function takes in 4 parameters
 *  of type array of int, and int
 *  This functions returns an int
 *
 *  This function recursively divides the array and returns the ceil of a number
 *  This is used to generate processes as per a probability distribution
 *
 */
int find_ceil(int arr[], int r, int l, int h) {
    int mid;
    while(l < h) {
        mid = l + ((h - l)>>1);
        (r > arr[mid]) ? (l = mid + 1) : (h = mid);
    }

    return (arr[l] >= r) ? l : -1;
}

/*  rand_gen()
 *
 *  This function takes in 4 arguments
 *  of type int
 *  This function returns an int
 *
 *  This funtion takes in percent of tasks present in CPU and returns an int
 *  indicating what type of process to generate. Accuracy of the distribution
 *  increases with the number of processes.
 *
 */
int rand_gen(int userIO, int userCPU, int systemIO, int systemCPU) {
    // Prefix array contains cdf
    int prefix[4];
    int arr[] = {0, 1, 2, 3};
    int freq[] = {userIO, userCPU, systemIO, systemCPU}; 

    // Create cdf
    prefix[0] = freq[0];
    int l = 0, h = 3;
    for(int i = 1; i < 4; i++) {
        prefix[i] = prefix[i - 1] + freq[i];
    }

    // Get rand
    int r = (rand() % prefix[3]) + 1;
    int indexc = find_ceil(prefix, r, l, h);
    return arr[indexc];               
}  

/*  create_proc()
 *
 *  This function takes in two arguments
 *  of type int
 *  This funciton returns a pointer to proc
 *
 *  This function takes in the process ID and process type to generate
 *  a process as described in sim.hm with rest of the fields populated by rand
 *
 */
proc* create_proc(int procID, int procType) {
    // Allocate memory for a process, and show failure in case malloc fails
    proc *new = (proc*)malloc(sizeof(proc));
    if(!new) {
        fprintf(stderr, "createProc error: malloc failure\n");
        exit(EXIT_FAILURE);
    }

    // Process parameters
    new->procID = procID;
    new->procType = procType;
    new->state = 0b01;
    new->arrivalTime = procID + rand()%5;
    
    if(procType == 2 || procType == 3){
        new->priority = 1 + rand()%100;
        if(procType == 3) 
            new->burstTime = 1 + rand()%3;
        else
            new->burstTime = 1 + rand()%5;
    } else {
        new->priority = 100 + rand()%40;
        new->burstTime = 1 + rand()%10;
    }
    
    if(procType == 0 || procType == 2) {
        new->burstTimeIO = new->burstTime>>1;
    } else {
        new->burstTimeIO = 0;
    }
    
    new->next = NULL;

    return new;
}

/*  insert_head()
 *
 *  This process takes in two parameters
 *  of type **proc and *proc
 *  This function returns nothing
 *
 *  This function inserts a proc node at head of a list
 *
 */
void insert_head(proc **list, proc *node) {
    node->next = *list;
    *list = node;
}

/*  Merge Sort on the Linked list   */
/*  merge_sort()
 *
 *  This function takes in one argument
 *  of type list of proc
 *  This function returns nothing
 *
 *  This function implements merge sort for a linked list
 *
 */
void merge_sort(proc **headRef) {
    proc *head = *headRef;
    proc *a;
    proc *b;

    // Base case
    if(head == NULL || head->next == NULL) {
        return;
    }

    // Split head into two sublists
    front_back_split(head, &a, &b);

    // Recursively sort sublists
    merge_sort(&a);
    merge_sort(&b);

    // Merge the two sorted lists
    *headRef = sorted_merge(a, b);
}

/*  front_back_split()
 *
 *  This function takes in three arguments
 *  of type process, and list of processes
 *  This function returns nothing
 *
 *  This function splits the list into two halves. In case of odd number of elements,
 *  the first list gets an extra element. It uses slow-fast pointer approach to 
 *  implement this.
 *
 */
void front_back_split(proc *source, proc **frontRef, proc **backRef){
    proc *fast;
    proc *slow;
    slow = source;
    fast = source->next;

    // Advance fast two nodes and slow one node
    while (fast != NULL) {
        fast = fast->next;
        if(fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // If fast is at NULL, slow is at midpoint
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

/*  sorted_merge()
 *
 *  This function takes two arguments
 *  of type proc*
 *  This function returns a proc*
 *
 *  This fuction implements merge part of merge sort
 *
 */
proc *sorted_merge(proc *a, proc*b) {
    proc *result = NULL;

    // Base case
    if(a == NULL)
        return b;
    else if(b == NULL)
        return a;

    // Pick either a or b, and recur
    if(a->arrivalTime <= b->arrivalTime) {
        result = a;
        result->next = sorted_merge(a->next, b);
    } else {
        result = b;
        result->next = sorted_merge(a, b->next);
    }
    return result;
}

/*  initmem()   TODO: obsolete, no description added
 */
int initmem(int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int *throughput, int *loadAverage) {
    memset(waitingTime, 0, numProc);
    memset(turnAroundTime, 0, numProc);
    memset(responseTime, 0, numProc);
    memset(cpuUtilisation, 0, numProc);
    memset(serviceTime, 0, numProc);
    throughput = 0;
    loadAverage = 0;
    return 0;
}

/*  mean()
 *
 *  This function takes in two arguments
 *  oftype array and int
 *  This function returns a float
 *
 *  This function implements mean of all the elements in an array
 *
 */
float mean(int *arr, int size) {
    float sum = 0.0;
    for(int i = 0; i < size; i++) {
        sum += arr[i];
    }

    return sum/(float)size;
}

/* Copy burstTime TODO: Obsolete
 */
int *copy_Btime(proc *head, int numProc){
    int *bTime = (int *)malloc(sizeof(int) * numProc);
    proc * temp = head;
    for(int i = 0; i < numProc; i++) {
        bTime[i] = temp->burstTime;
        temp = temp->next;
    }
    return bTime;
}

/*  FCFS
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements FCFS algorithm, updating at every second
 *
 */
int fcfs(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {
    // Reset memory
    initmem(numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, &throughput, &loadAverage);
    
    int currentTime = 0;
    proc *temp = head;
    proc *curr = head;
    int numTerm = 0;

    int running = 0;
    int runTime = 0;
    head->state = 0b10;
    // While number of terminated process is less than number started
    while(numTerm < numProc) {
        printf("CurrentTime: %d\n", currentTime);
        temp = head;
        while(temp != NULL) {
            // While currentTime is greater than arrivaltime 
            if(currentTime >= temp->arrivalTime) {
                if(running == 0 && temp->state == 0b10) {
                    curr = temp;
                    running = temp->procID;
                    temp->state = 0b11;
                    runTime = temp->burstTime + currentTime;
                    printf("Currently running: %d with runtime: %d\n", temp->procID, runTime);
                }
                // Execute till burstTime
                else {
                    if(temp->state == 0b01) {
                        temp->state = 0b10;
                    }
                }
                if(temp->state == 0b10) {
                    printf("Ready: %d\n", temp->procID); 
                    waitingTime[temp->procID - 1]++;
                    loadAverage++;
                }
            } 
            temp = temp->next;
        }
        currentTime++;
        printf("runtime: %d, currentTime: %d\n", runTime, currentTime);
        
        // Terminate the process
        if(runTime == currentTime) {
            if(curr != NULL) {
                running = 0;
                turnAroundTime[curr->procID - 1] = currentTime - curr->arrivalTime;
                serviceTime[curr->procID - 1] = waitingTime[curr->procID - 1] + curr->arrivalTime;
                curr->state = 0b00;
                printf("Terminated: %d\n", curr->procID);
                numTerm++;
            }
        }
    }

    // Calculate Stats
    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);

    // Print Statistics
    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc/(float)currentTime);
    printf("CPU Utilisation: 1\n"); 
    
    return 0;
}

/*  SJF
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements SJF algorithm, updating at every second
 *
 */
int sjf(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {
    initmem(numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, &throughput, &loadAverage);
    
    int currentTime = 0;
    int running = 0;
    int start = 0;
    int numTerm = 0;
    proc *minp = NULL;
    
    // While numTerm < numProc
    while(numTerm < numProc) {
        proc *temp = head;
        // If no process is ready
        if(running == 0 && temp != NULL && currentTime < temp->arrivalTime){
            start++;
            currentTime++;
            continue;
        }
        
        // Set processes as ready
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b01) {
                printf("proc %d ready\n", temp->procID);
                temp->state = 0b10;
            }
            if(temp->state == 0b10)
                waitingTime[temp->procID - 1]++;
            temp = temp->next;
        }
        
        // Get number of ready processes per clock
        int ready = ready_proc(head, currentTime);
        loadAverage += ready;
        printf("currentTime: %d numReady: %d\n", currentTime, ready);
        
        // Get process with min burstTime, 
        int min = INT_MAX;
        temp = head;
        if(running == 0) {
            while(temp != NULL && currentTime >= temp->arrivalTime) {
                if(temp->state == 0b10 && min > temp->burstTime) {
                    min = temp->burstTime;
                    minp = temp;
                }
                temp = temp->next;
            }
            printf("minProc = %d,\tminBurst = %d\n", minp->procID, minp->burstTime);
        }
       
        // Pick a process to run depending on shortest time
        if(running == 0) {
            printf("currently running: %d, with burstTIme: %d\n", minp->procID, minp->burstTime);
            minp->state = 0b11;
            start = currentTime;
            running = 1;
            minp->burstTime++;
        }
    
        // Run that process till its burst is complete.
        if(running == 1) {
            if(minp->burstTime > 1){
                minp->burstTime--;
                printf("currently running: %d, with remaining burstTIme: %d\n", minp->procID, minp->burstTime);
            } else {
                printf("terminating : %d\n", minp->procID);
                running = 0;
                minp->state = 0b00;
                turnAroundTime[minp->procID - 1] = currentTime - minp->arrivalTime;
                serviceTime[minp->procID - 1] = waitingTime[minp->procID - 1] + minp->arrivalTime;
                numTerm++;
                minp = NULL;
            }
        }
        currentTime++;
    }

    // Calculate Stats
    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);

    // Display the stats
    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc/(float)currentTime);
    printf("CPU Utilisation: 1\n"); 
    
    return 0;
}

/*  priority1()
 *
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements priority algorithm with ageing, updating at every second
 *
 */
int priority1(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {
    initmem(numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, &throughput, &loadAverage);
    
    int currentTime = 0;
    int running = 0;
    int start = 0;
    int numTerm = 0;
    proc *minp = NULL;
    
    // While numTerm < numProc
    while(numTerm < numProc) {
        proc *temp = head;
        // If no process is ready
        if(running == 0 && temp != NULL && currentTime < temp->arrivalTime){
            start++;
            currentTime++;
            continue;
        }
        
        // Set processes as ready
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b01) {
                printf("proc %d ready\n", temp->procID);
                temp->state = 0b10;
            }
            if(temp->state == 0b10)
                waitingTime[temp->procID - 1]++;
            temp = temp->next;
        }
        
        // Get number of ready processes per clock
        int ready = ready_proc(head, currentTime);
        loadAverage += ready;
        printf("currentTime: %d numReady: %d\n", currentTime, ready);
        
        // Get process with highest priority (lower number has higher priority), 
        int min = INT_MAX;
        temp = head;
        if(running == 0) {
            while(temp != NULL && currentTime >= temp->arrivalTime) {
                if(temp->state == 0b10 && min > temp->priority) {
                    min = temp->priority;
                    minp = temp;
                }
                temp = temp->next;
            }
            printf("minProc = %d,\tminPrioriy = %d\n", minp->procID, minp->priority);
        }
       
        // Pick a process to run depending on shortest time
        if(running == 0) {
            printf("currently running: %d, with priority: %d\n", minp->procID, minp->priority);
            minp->state = 0b11;
            start = currentTime;
            running = 1;
        }

        // Run that process till its burst is complete.
        if(running == 1) {
            if(minp->burstTime > 1){
                minp->burstTime--;
                printf("currently running: %d, with remaining burstTIme: %d\n", minp->procID, minp->burstTime);
                // Ageing
                ageing(head, minp, 0);
            } else {
                printf("terminating : %d\n", minp->procID);
                running = 0;
                minp->state = 0b00;
                turnAroundTime[minp->procID - 1] = currentTime - minp->arrivalTime;
                serviceTime[minp->procID - 1] = waitingTime[minp->procID - 1] + minp->arrivalTime;
                numTerm++;
                minp = NULL;
            }
        }
        currentTime++;
    }

    // Calculate Stats
    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);

    // Display stats
    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc/(float)currentTime);
    printf("CPU Utilisation: 1.00\n"); 
    
    return 0;
}

/*  Round Robin
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements RR algorithm, updating at every second
 *
 */
int round_robin(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage, int quanta) {
    initmem(numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, &throughput, &loadAverage);
    
    int currentTime = 0;
    int running = 0;
    int numTerm = 0;
    proc *curr = head;
    int minBurst = quanta;
    int start = 0;

    while(numTerm < numProc) {
        proc * temp = head;

        // If no process is ready
        if(running == 0 && temp != NULL && currentTime < temp->arrivalTime) {
            start++;
            currentTime++;
            continue;
        }

        // Set processes as ready
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b01) {
                printf("proc %d ready\n", temp->procID);
                temp->state = 0b10;
            }
            if(temp->state == 0b10)
                waitingTime[temp->procID - 1]++;
            temp = temp->next;
        }

        // Get number of ready processes per clock
        int ready = ready_proc(head, currentTime);
        loadAverage += ready;
        printf("CurrentTime: %d, numReady: %d\n", currentTime, ready);

        if(curr == NULL) {
            curr = head;
        }

        // Find a runnable process as per fcfs
        // If there is atleast one ready process, find it
        if(ready != 0 && running == 0) {
            while(curr != NULL) {
                // Find the first ready process
                if(curr->state == 0b10) {
                    printf("Starting %d with burstTime: %d\n", curr->procID, curr->burstTime);
                    curr->state = 0b11;
                    running = 1;
                    break;
                } else {
                    // Loop for all processes that have arrived
                    if(curr->next->arrivalTime < currentTime) {
                        curr = curr->next;
                    } // If this last process in queue, loop over and search next ready from head
                    else {
                        curr = head;
                    }
                }
            }
        }

        // Run this process for minimum of (quanta, burstTime) amount of time
        if(running == 1) {
            minBurst = min(minBurst, curr->burstTime);
            if(minBurst >= 1) {
                printf("Currently running %d for %d with burstTime %d\n", curr->procID, minBurst, curr->burstTime);
                minBurst--;
                curr->burstTime--;
            } else {
                if(curr->burstTime == 0) {
                    printf("Terminating: %d\n", curr->procID);
                    running = 0;
                    curr->state = 0b00;
                    minBurst = quanta;
                    turnAroundTime[curr->procID - 1] = currentTime - curr->arrivalTime;
                    serviceTime[curr->procID - 1] = waitingTime[curr->procID - 1] + curr->arrivalTime;
                    numTerm++;
                    currentTime--;
                }
                if(minBurst == 0 && curr->burstTime != 0) {
                    printf("Switching from process %d\n", curr->procID);
                    curr->state = 0b10;
                    running = 0;
                    minBurst = quanta;

                    // Add Switiching overhead of one clock
                    currentTime++;
                }
               
                // Increment current to next process, such that when next runnable process is searched for, this process is last one to be found
                if(curr->next != NULL) {
                    curr = curr->next;
                } else {
                    curr = head;
                }
            }
        }

        // if curr reaches null, curr = head and repeat
        currentTime++;
    }

    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);

    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc / (float)currentTime); 
    printf("CPU Utilisation: %f\n", 1 - (float)start/(float)currentTime); 

    return 0;
}

/*  Priority2
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements FCFS algorithm, updating at every second
 *
 */
int priority2(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {
    initmem(numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, &throughput, &loadAverage);
   
    int start = 0;
    int currentTime = 0;
    int numTerm = 0;
    proc *minp = NULL;
    
    // While numTerm < numProc
    while(numTerm < numProc) {
        proc *temp = head;
        
        // Set processes as ready
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b01) {
                printf("proc %d ready\n", temp->procID);
                temp->state = 0b10;
            }
            if(temp->state == 0b10)
                waitingTime[temp->procID - 1]++;
            temp = temp->next;
        }
        
        // Get number of ready processes per clock
        int ready = ready_proc(head, currentTime);
        loadAverage += ready;
        printf("currentTime: %d numReady: %d\n", currentTime, ready);
        
        // If no process is ready
        if(!ready) {
            start++;
            currentTime++;
            continue;
        }

        // Get process with highest priority (lower number has higher priority), 
        int min = INT_MAX;
        temp = head;
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b10 && min >= temp->priority) {
                min = temp->priority;
                minp = temp;
            }
            temp = temp->next;
        }
        printf("minProc = %d,\tminPrioriy = %d\n", minp->procID, minp->priority);
       
        // Pick a process to run depending on highest priority
        printf("currently running: %d, with priority: %d\n", minp->procID, minp->priority);
        
        // If the process has burstTime > 1.
        if(minp->burstTime > 1){
            minp->burstTime--;
            printf("currently running: %d, with remaining burstTIme: %d\n", minp->procID, minp->burstTime);
            
            // Context switch overhead
            currentTime++;
        }
        // Else terminate it
        else {
            printf("terminating : %d\n", minp->procID);
            minp->state = 0b00;
            turnAroundTime[minp->procID - 1] = currentTime - minp->arrivalTime;
            serviceTime[minp->procID - 1] = waitingTime[minp->procID - 1] + minp->arrivalTime;
            numTerm++;
            minp = NULL;
        }
        ageing(head, minp, 0);
        currentTime++;
    }

    // Calculate Stats
    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);

    // Display stats
    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc / (float)currentTime);
    printf("CPU Utilisation: %f\n", 1 - (float)start / (float) currentTime); 
    
    return 0;
}

/*  m_level()
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements multi level queue algorithm, updating at every second
 *
 */
int m_level(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {

    // Divide jobs into queues
    proc *L1Proc = NULL;
    proc *L2Proc = NULL;
    proc *L3Proc = NULL;
    proc *temp = NULL;

    // Remove a process from tail and add it to head of appropriate queue
    while(head != NULL) {
        temp = delete_tail(head);
        if(temp->priority < 50) {
            insert_head(&L1Proc, temp);
        }
        else if (temp->priority >= 50 && temp->priority < 100) {
            insert_head(&L2Proc, temp);
        }
        else {
            insert_head(&L3Proc, temp);
        }
        
        if(head->next == NULL){
            if(head->priority < 50) {
                insert_head(&L1Proc, head);
            }
            else if (head->priority >= 50 && head->priority < 100) {
                insert_head(&L2Proc, head);
            }
            else {
                insert_head(&L3Proc, head);
            }
            break;
        }
    }

    // Display the queues
    printf("Multilevel queues\n");
    printf("\n\nL1\n");
    disp_list(L1Proc);
    printf("\n\nL2\n");
    disp_list(L2Proc);
    printf("\n\nL3\n");
    disp_list(L3Proc);

    // Use FCFS among queues
    int currentTime = 0;
    int numTerm = 0;
    int readyL1 = 0, readyL2 = 0, readyL3 = 0;
    int running = 0;
    proc *curr = NULL;
    int start = 0;

    while(numTerm < numProc) {
        // Set processes as ready
        int n = 0;
        proc *temp = NULL;
        while(n < 3) {
            if(n == 0) {
                temp = L1Proc;
            } else if (n == 1) {
                temp = L2Proc;
            } else if (n == 2) {
                temp = L3Proc;
            }

            while(temp != NULL && currentTime >= temp->arrivalTime) {
                if(temp->state == 0b01) {
                    printf("proc %d ready\n", temp->procID);
                    temp->state = 0b10;
                }
                if(temp->state == 0b10)
                    waitingTime[temp->procID - 1]++;
                temp = temp->next;
            }
            n++;
        }
        
        // Get number of ready processes in each queue
        readyL1 = ready_proc(L1Proc, currentTime);
        readyL2 = ready_proc(L2Proc, currentTime);
        readyL3 = ready_proc(L3Proc, currentTime);
       
        printf("CurrentTime: %d\n", currentTime);
        printf("Ready L1: %d\n", readyL1);
        printf("Ready L2: %d\n", readyL2);
        printf("Ready L3: %d\n", readyL3);
        loadAverage += readyL1 + readyL2 + readyL3;

        // If there is no job running, scan queues in priority
        if(running == 0) {
            // Pick a ready task from L1
            if(readyL1) {
                curr = L1Proc;
            }
            // Pick a ready task from L2
            else if (readyL2) {
                curr = L2Proc;
            }
            // Pick a ready task from L3
            else if (readyL3) {
                curr = L3Proc;
            }
            // If there is no ready process, continue
            else {
                start++;
                currentTime++;
                continue;
            }

            // Get next ready process 
            int min = INT_MAX;
            proc *minp = curr;
            while(minp != NULL && currentTime >= minp->arrivalTime) {
                if(minp->state == 0b10 && min > minp->priority) {
                    curr = minp;
                    min = minp->priority;
                }
                minp = minp->next;
            }
        }
        
        // Age the processes in queue
        ageing(L1Proc, curr, 0);
        ageing(L2Proc, curr, 0);
        ageing(L3Proc, curr, 0);
        
        // Pick this process 
        if(running == 0) {
            printf("Currently running %d with priority: %d, and burstTime: %d\n", curr->procID, curr->priority, curr->burstTime);
            curr->state = 0b11;
            running = 1;
        }

        // Run this process till its burst is complete
        if(running == 1) {
            if(curr->burstTime > 1) {
                curr->burstTime--;
                printf("Currently running %d, remaining burstTime: %d\n", curr->procID, curr->burstTime);
            } else {
                printf("Terminating %d\n", curr->procID);
                running = 0;
                curr->state = 0b00;
                turnAroundTime[curr->procID - 1] = currentTime - curr->arrivalTime;
                serviceTime[curr->procID - 1] = waitingTime[curr->procID - 1] + curr->arrivalTime;
                numTerm++;
            }
        }
    // Use priority in a queue
        currentTime++;
    }

    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);

    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc / (float)currentTime);
    printf("CPU Utilisation: %f\n", 1 - (float)start / (float)currentTime); 

    return 0;
}

/*  mlf_queue()
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements multi level feedback queue algorithm, updating at every second
 *
 */
int mlf_queue(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {

    // Divide jobs into queues
    proc *L1Proc = NULL;
    proc *L2Proc = NULL;
    proc *L3Proc = NULL;
    proc *temp = NULL;

    // Remove a process from tail and add it to head of appropriate queue
    while(head != NULL) {
        temp = delete_tail(head);
        if(temp->priority < 50) {
            insert_head(&L1Proc, temp);
        }
        else if (temp->priority >= 50 && temp->priority < 100) {
            insert_head(&L2Proc, temp);
        }
        else {
            insert_head(&L3Proc, temp);
        }
        
        if(head->next == NULL){
            if(head->priority < 50) {
                insert_head(&L1Proc, head);
            }
            else if (head->priority >= 50 && head->priority < 100) {
                insert_head(&L2Proc, head);
            }
            else {
                insert_head(&L3Proc, head);
            }
            break;
        }
    }

    
    printf("Multilevel queues\n");
    printf("\n\nL1\n");
    disp_list(L1Proc);
    printf("\n\nL2\n");
    disp_list(L2Proc);
    printf("\n\nL3\n");
    disp_list(L3Proc);

    // Use FCFS among queues
    int currentTime = 0;
    int numTerm = 0;
    int readyL1 = 0, readyL2 = 0, readyL3 = 0;
    int running = 0;
    proc *curr = NULL;
    int start = 0;

    while(numTerm < numProc) {
        // Set processes as ready
        int n = 0;
        proc *temp = NULL;
        while(n < 3) {
            if(n == 0) {
                temp = L1Proc;
            } else if (n == 1) {
                temp = L2Proc;
            } else if (n == 2) {
                temp = L3Proc;
            }

            while(temp != NULL && currentTime >= temp->arrivalTime) {
                if(temp->state == 0b01) {
                    printf("proc %d ready\n", temp->procID);
                    temp->state = 0b10;
                }
                if(temp->state == 0b10)
                    waitingTime[temp->procID - 1]++;
                temp = temp->next;
            }
            n++;
        }
       
        // Get number of ready processes in each queue
        readyL1 = ready_proc(L1Proc, currentTime);
        readyL2 = ready_proc(L2Proc, currentTime);
        readyL3 = ready_proc(L3Proc, currentTime);
        loadAverage += readyL1 + readyL2 + readyL3;

        printf("CurrentTime: %d\n", currentTime);
        printf("Ready L1: %d\n", readyL1);
        printf("Ready L2: %d\n", readyL2);
        printf("Ready L3: %d\n", readyL3);

        // If there is no job running, scan queues in priority
        if(running == 0) {
            // Pick a ready task from L1, highest priority
            if(readyL1) {
                curr = L1Proc;
            }
            // Pick a ready task from L2, normal priority
            else if (readyL2) {
                curr = L2Proc;
            }
            // Pick a ready task from L3, low priority
            else if (readyL3) {
                curr = L3Proc;
            }
            // If there is no ready process, continue
            else {
                start++;
                currentTime++;
                continue;
            }

            // Get next ready process 
            int min = INT_MAX;
            proc *minp = curr;
            while(minp != NULL && currentTime >= minp->arrivalTime) {
                if(minp->state == 0b10 && min > minp->priority) {
                    curr = minp;
                    min = minp->priority;
                }
                minp = minp->next;
            }
        }
       
        // Age the processes in queue
        ageing(L1Proc, curr, 1);
        ageing(L2Proc, curr, 1);
        ageing(L3Proc, curr, 1);
       
        // Pick this process 
        if(running == 0) {
            printf("Currently running %d with priority: %d, and burstTime: %d\n", curr->procID, curr->priority, curr->burstTime);
            curr->state = 0b11;
            running = 1;
        }

        // Run this process till its burst is complete
        if(running == 1) {
            if(curr->burstTime > 1) {
                curr->burstTime--;
                printf("Currently running %d, remaining burstTime: %d\n", curr->procID, curr->burstTime);
                //turnAroundTime[curr->procID - 1]++;
            } else {
                printf("Terminating %d\n", curr->procID);
                running = 0;
                curr->state = 0b00;
                turnAroundTime[curr->procID - 1] = currentTime - curr->arrivalTime;
                serviceTime[curr->procID - 1] = waitingTime[curr->procID - 1] + curr->arrivalTime;
                numTerm++;
            }
        }

        // Check processes if they cross threshold priorities. L1Proc with system processes is taken to be off limits, processes can only move between L2Proc and L3Proc
        proc *check = L2Proc;
        while(check != NULL) {
            if(check->state != 0b00 && check->priority >= 100) {
                printf("Moving %d from L2 to L3\n", check->procID); 
                proc *temp = check->next;         
                sorted_insert(&L3Proc, delete_i(&L2Proc, check));
                //check = L2Proc;
                check = temp;
                continue;
            } else 
                check = check->next;
        }
        
        check = L3Proc;
        while(check != NULL) {
            if(check->state != 0b00 && check->priority < 100) {
                printf("Moving %d from L3 to L2\n", check->procID); 
                proc *temp = check->next;
                sorted_insert(&L2Proc, delete_i(&L3Proc, check));
                //check = L3Proc;
                check = temp;
                continue;
            } else 
            check = check->next;
        }
        
        // Updated Process queues
    //    printf("\nL2\n");
    //    disp_list(L2Proc);
    //    printf("\nL3\n");
    //    disp_list(L3Proc);
                
        currentTime++;
    }

    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);
    
    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc / (float)currentTime);
    printf("CPU Utilisation: %f\n", 1 - (float)start/(float)currentTime); 

    return 0;
}

/*  SRTF
 *
 *  This function takes 9 arguments
 *  of type int, int*
 *  This function returns nothing
 *
 *  This function implements SRTF algorithm, updating at every second
 *
 */
int srtf(proc *head, int numProc, int *waitingTime, int *turnAroundTime, int *responseTime, int *cpuUtilisation, int *serviceTime, int throughput, int loadAverage) {
    initmem(numProc, waitingTime, turnAroundTime, responseTime, cpuUtilisation, serviceTime, &throughput, &loadAverage);
    
    int start = 0;
    int currentTime = 0;
    int numTerm = 0;
    proc *minp = NULL;

    // While numTerm < numProc
    while(numTerm < numProc) {
        proc *temp = head;
        
        // Set processes as ready
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b01) {
                printf("proc %d ready\n", temp->procID);
                temp->state = 0b10;
            }
            if(temp->state == 0b10)
                waitingTime[temp->procID - 1]++;
            temp = temp->next;
        }
        
        // Get number of ready processes per clock
        int ready = ready_proc(head, currentTime);
        loadAverage += ready;
        printf("currentTime: %d numReady: %d\n", currentTime, ready);
       
        // If no process is ready
        if(!ready) {
            start++;
            currentTime++;
            continue;
        }

        // Get process with min burstTime, 
        temp = head;
        int min = INT_MAX;
        while(temp != NULL && currentTime >= temp->arrivalTime) {
            if(temp->state == 0b10 && min >= temp->burstTime) {
                min = temp->burstTime;
                minp = temp;
            }
            temp = temp->next;
        }
        printf("minProc = %d,\tminBurst = %d\n", minp->procID, minp->burstTime);
       
        // Pick a process to run depending on shortest time
        printf("currently running: %d, with burstTIme: %d\n", minp->procID, minp->burstTime);
        
       // If process has burst time > 1 
        if(minp->burstTime > 1){
            minp->burstTime--;
            printf("currently running: %d, with remaining burstTIme: %d\n", minp->procID, minp->burstTime);
            turnAroundTime[minp->procID - 1]++;
        }
        // Else terminate it cycle
        else {
            printf("terminating : %d\n", minp->procID);
            minp->state = 0b00;
            turnAroundTime[minp->procID - 1] = currentTime - minp->arrivalTime;
            serviceTime[minp->procID - 1] = waitingTime[minp->procID - 1] + minp->arrivalTime;
            numTerm++;
            minp = NULL;
        }
        currentTime++;
    }

    float avWait = mean(waitingTime, numProc);
    float avTAT = mean(turnAroundTime, numProc);
    float avResp = mean(waitingTime, numProc);
    float avServ = mean(serviceTime, numProc);
    
    printf("\nStatistics\n");
    printf("average Waiting TIme: %f\n", avWait);
    printf("average TAT: %f\n", avTAT);
    printf("average Response Time: %f\n", avResp);
    printf("average Service Time: %f\n", avServ);
    printf("load average: %f\n", (float)loadAverage / (float)currentTime);
    printf("Throughput: %f\n", (float)numProc / (float)currentTime);
    printf("CPU Utilisation: %f\n", 1 - (float)start/(float)currentTime); 

    return 0;
}

/*  ready_proc()
 *
 *  This function takes in two arguments
 *  of type proc and int
 *  This function returns an int
 *
 *  This funtion returna the number of ready processes at a given
 *  instant of time
 *
 */
int ready_proc(proc *head, int currentTime) {
    // Initialisation
    int count = 0;
    proc * temp = head;

    // While there are processes that have arrived till currentTim, traverse the list
    while (temp != NULL && currentTime >= temp->arrivalTime) {
        if(temp->state == 0b10) {
            // Increment the count of ready processes
            count++;
        }
        temp = temp->next;
    }

    // Return the number of ready processes
    return count;
}

/*  set_state(): Obsolete
 */
// Reset the state of process to waiting
int set_state(proc * head) {
    proc * temp = head;
    while(temp != NULL) {
        temp->state = 0b01;
        temp = temp->next;
    }
    return 0;
}

/*  copy()
 *
 *  This function takes in single argument
 *  of type proc*
 *  This funciton returns a proc*
 *
 *  This function recursively copies a linked list
 *
 */
proc *copy(proc *head) {
    // If head is null, nothing to copy
    if(head == NULL)
        return NULL;

    // Create new proc struct to hold copied values
    proc *new = (proc*)malloc(sizeof(proc));

    // Copy the values
    new->procID = head->procID;
    new->procType = head->procType;
    new->state = head->state;
    new->arrivalTime = head->arrivalTime;
    new->priority = head->priority;
    new->burstTime = head->burstTime;
    new->burstTimeIO = head->burstTimeIO;

    // Recursive call to copy next process
    new->next = copy(head->next);
    
    // Return new process
    return new;
}

/*  delete_tail()
 *
 *  This function takes in single argument
 *  of type list
 *  This funtction returns a proc
 *
 *  This function retrieves an element at tail of a linked list
 *  and removes it from the list
 *
 */
proc *delete_tail(proc *list) {
    proc *temp = list;
    // Traverse to the tail
    while(temp->next->next != NULL) {
        temp = temp->next;
    }

    // Remove the process from tail
    proc *new = temp->next;
    temp->next = NULL;
    return new;
}

/*  min()
 *
 *  This function takes two arguments
 *  of type int
 *  This function returns an int
 *
 *  This function takes two ints and returns the smaller of the two
 *
 */
int min(int num1, int num2) {
    return (num1 < num2 ? num1 : num2);
}

/*  Ageing()
 *
 *  This function takes three arguments
 *  of type proc*, and int
 *  This function returns an int
 *
 *  This function implements ageing of process in the list
 *  If a process has been waiting, its priority is increased,
 *  If it has been running, its priority is slowly decreased.
 *
 */
int ageing(proc *head, proc *running, int flag) {
    // If head is NULL, nothing to age
    if(head == NULL) {
        return 0;
    }
    
    // If head is not the running process, age it
    if(head->state == 0b10 && head != running) {
        head->priority -= 10;
        if(head->priority <= 0){
            head->priority = 1;
        }

        // Maintain user and system priority boundries
        if(flag == 0 && head->priority >= 90 && head->priority < 100){
            head->priority = 100;
        }

        if(flag == 1 && head->priority >= 40 && head->priority < 50){
            head->priority = 50;
        }

        printf("Increased priority of proc %d to %d\n", head->procID, head->priority);
    }

    // Decrease priority of currently running process
    if(head == running) {
        head->priority += 5;
        printf("Decreased priority of proc %d to %d\n", head->procID, head->priority);
    }

    // Recursive call to next process in list
    return ageing(head->next, running, flag);
}

/*  delete_i()
 *
 *  This function takes two arguments 
 *  of type proc** and proc *
 *  This function returns a proc*
 *
 *  This function implements deleteion of an arbitrary node
 *  from the linked list
 *
 */
proc *delete_i(proc **head, proc *process) {
    proc *temp = *head;
    proc *prev;

    // If head node itself is teh process to be deleted
    if(temp != NULL && process == temp) {
        *head = temp->next;
        temp->next = NULL;
        return temp;
    }

    // Search the key to be deleted
    while(temp != NULL && temp != process){
        prev = temp;
        temp = temp->next;
    }

    // If key is not present
    if(temp == NULL)
        return 0;

    prev->next = temp->next;

    temp->next = NULL;
    return temp;
}

/*  sorted_insert()
 *
 *  This function takes in two arguments
 *  of type proc** and proc*
 *  This function returns nothing
 *
 *  This function implements sorted insertion in a sorted linked list
 *
 */
// Inserted sort in an linked list
void sorted_insert(proc **head, proc *process) {
    proc *current;

    // For insert at head
    if(*head == NULL || (*head)->arrivalTime >=process->arrivalTime) {
        process->next = *head;
        *head = process;
    }
    else {
        // Search the node location before which current will be inserted
        current = *head;
        while(current->next != NULL && current->next->arrivalTime < process->arrivalTime) {
            current = current->next;
        }
        process->next = current->next;
        current->next = process;
    }
}
