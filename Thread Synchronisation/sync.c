/*  sync.c
 *
 *  Ankit Gola                  :       2017EET2296
 *
 *  This is the sync.c file. It contains all the function definitions used in main
 *
 */

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// Program specific header file
#include "sync.h"

// Global defaults
int maxProcess ;                    // Maximum queue length
int maxServicemen ;                 // Maximum number of servicemen
int activeSmen = 0;                 // Currently active servicemen
int serviceStatus[MAXMAXS];         // Array to store status of servicemen
int uid = 1;

int maxArrivalTime = 5;             // Ceil on rand() for arrival time
int maxUseTime = 5;                 // Ceil on rand() for use time
int ex = 0;                         // Exit flag

USER *list = NULL;                  // Main queue list
USER *processing = NULL;            // Queue for USERS under process

/*  Display a linked list
 *
 *  This function takes in one argument
 *  of type USER *
 *  This function returns nothing
 *
 *  This function displays the information of the USER nodes in a list
 */
void display(USER *list) {
    // If list is empty
    if(list == NULL){
        printf("NULL\n");
        return;
    }

    while(list != NULL) {
        // If user is being serviced,
        if(list->serviceMan != -1) {
            printf(MAGENTA "Serviceman %d is servicing the following USER\n", list->serviceMan);
        }
        // else
        else {
            printf(YELLOW);
        }
        // Print USER info
        printf("UID             :       %d\n", list->uid);
        printf("arrivalTime     :       %d\n", list->arrivalTime);
        printf("useTime         :       %d\n", list->useTime);
        printf("state           :       %d\n", list->state);
        list = list->next;
        printf("\n");
    }
    printf("NULL\n");
    printf(RESET);
}

/*  create_user()
 *
 *  This function takes one argument
 *  of type int
 *  This function returns one USER*
 *
 *  This function takes the uid, and creates a USER, and returns it.
 */
USER *create_user(int uid) {
    // Allocate memory for the new user
    USER *new = (USER *)malloc(sizeof(USER));
    // If malloc fails
    if(!new) {
        fprintf(stderr, BOLD RED "create_user: malloc failure\n" RESET);
        exit(EXIT_FAILURE);
    }

    // User parameters
    new->uid = uid;
    new->arrivalTime = uid + rand()%maxArrivalTime;
    new->useTime = 1 + rand()%maxUseTime;
    new->state = 0b10;
    new->serviceMan = -1;
    new->next = NULL;

    // Return USER
    return new;
}

/*  init()
 *
 *  This function takes in one argument
 *  of type int
 *  This function returns nothing
 *
 *  This functions spawns a number of USERs and adds them 
 *  into a queue in sorted manner
 */
void init(int size) {
    for(int i = 0; i < size; i++) {
        sorted_insert(&list, i);
    }
    uid += size;
}

/*  sorted_insert2()
 *
 *  This function takes two arguments
 *  of type USER ** and USER *
 *  This function returns nothing
 *
 *  This function inserts into a list, a USER node in ascending order
 *  of the serviceman servicing it.
 */
void sorted_insert2(USER **list, USER *user){
    USER *current;

    // For insert at head
    if(*list == NULL || (*list)->serviceMan >= user->serviceMan) {
        user->next = *list;
        *list = user;
    }
    else {
        // Search the node location before which current will be inserted
        current = *list;
        while(current->next != NULL && current->next->serviceMan <= user->serviceMan){
            current = current->next;
        }
        // Insert
        user->next = current->next;
        current->next = user;
    }
}

/*  sorted_insert()
 *
 *  This function takes two arguments
 *  of type USER ** and int
 *  This function returns nothing
 *
 *  This function takes a uid, creates a node using create_node, and inserts
 *  the USER in list in ascending order of arrival time.
 */
void sorted_insert(USER **list, int uid){
    USER *current;

    // Create a new USER node
    USER *new = create_user(uid);

    // For insert at head
    if(*list == NULL || (*list)->arrivalTime >= new->arrivalTime) {
        new->next = *list;
        *list = new;
    }
    else {
        // Search the node location before which current will be inserted
        current = *list;
        while(current->next != NULL && current->next->arrivalTime <= new->arrivalTime){
            current = current->next;
        }
        
        // Insert
        new->next = current->next;
        current->next = new;
    }
}

/*  random_remove()
 *
 *  This funciton takes two arguments
 *  of type USER ** and int
 *  This function returns a USER*
 *
 *  This function takes a uid, searches for it in the list,
 *  and if found, removes it from the list and returns it.
 *  This implementation is thread safe. 
 *  For example, if thread x is removing node 5, 
 *  then thread y can remove node 2, but not node 4.
 */
USER *random_remove(USER **head, int uid) {
    USER *prev = *head;
    USER *temp;

    // Lock the head
    pthread_mutex_lock(&prev->lock);

    // If head itself is the process to be deleted
    if(prev != NULL && prev->uid == uid) {
        *head = prev->next;
        prev->next = NULL;
        pthread_mutex_unlock(&prev->lock);
        return prev;
    }

    // Search the key to be deleted
    while((temp = prev->next) != NULL) {
        pthread_mutex_lock(&temp->lock);
        // If match found
        if(temp->uid == uid) {
            prev->next = temp->next;
            temp->next = NULL;
            pthread_mutex_unlock(&temp->lock);
            pthread_mutex_unlock(&prev->lock);
            return(temp);
        }
        pthread_mutex_unlock(&prev->lock);
        prev = temp;
    }

    // If key is not present
    if(temp == NULL) {
        printf(BOLD RED "Error: This user %d does not exist. Probably a glitch in the Matrix\n" RESET, uid);
        return 0;
    }

    pthread_mutex_unlock(&prev->lock);
    return NULL;
}

/*  get_count()
 *
 *  This function takes in one argument
 *  of type USER*
 *  Thsi function return and int
 *
 *  This function counts the number of USERS in a linked list
 */
int get_count(USER *list){
    int count = 0;
    for(; list != NULL; list = list->next) {
        count++;
    }
    return count;
}

/*  servicemen()
 *
 *  This process takes no arguments
 *  This process returns a void*
 *
 *  This function hanldes the possible tasks for a serviceman.
 */
void *servicemen() {
    int activeU = 0;                    // Keeps track of Number of USERS in list
    long currentServiceMen = 0;         // Max number of current servicemen
    
    pthread_t UID[MAXMAXS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Initialise servicemen status
    for(int i = 0; i < MAXMAXS; i++) {
        serviceStatus[i] = 0;
    }
    
    while(1) {
        // Get number of USERS
        activeU = get_count(list);

        // Spawn a serviceman thread
        while(currentServiceMen < maxServicemen && activeU > currentServiceMen){
            pthread_create(UID+currentServiceMen, &attr, processUser, (void*)currentServiceMen);
            serviceStatus[currentServiceMen] = 1;
            currentServiceMen++;
            activeSmen++;
        }
        
        // Reduce active servicemen and join threads.
        while(currentServiceMen > maxServicemen) {
            currentServiceMen--;
            activeSmen--;
            serviceStatus[currentServiceMen] = -1;
            pthread_join((long)UID+currentServiceMen, NULL);
        }

        // Waking up service men
        while (currentServiceMen > activeSmen && activeU > activeSmen) {
            for(int i = 0; i < currentServiceMen; i++) {
                if(serviceStatus[i] == 0) {
                    printf(BOLD MAGENTA "Service man %d waking up\n" RESET, i);
                    serviceStatus[i] = 1;
                    activeSmen++;
                    break;
                }
            }
        }

        // If exit flag is set
        if(ex) {
            // If there are any users in list, they will be dropped
            activeU = get_count(list);
            if(activeU){
                printf(BOLD CYAN "Time to close for today. Come back tomorrow. Dropping following users in ready queue\n" RESET);
                display(list);
                list = NULL;
                printf("\n");
                // Any users currently being processed will be serviced till completion
                printf(BOLD CYAN "Wrapping up with following users in processing queue\n" RESET);
                display(processing);
                while(processing != NULL);
            }
            break;
        }
    }
    return 0;
}

/*  processUser()
 *
 *  This process takes one argument
 *  of type void*
 *  This function returns a void *
 *
 *  This function finds a serviceable USER and services it
 */
void *processUser (void *serviceManID) {
    long sid = (long)serviceManID;
   
    while(1) {
        if(serviceStatus[sid] == 1) {
            USER *temp = list;
            
            // Look for a runnable user
            while(temp != NULL && pthread_mutex_trylock(&temp->lock) != 0) {
                temp = temp->next;
            }

            // If queue is empty, send serviceman to sleep
            if(temp == NULL) {
                printf(BOLD BLUE "Serviceman %ld going to sleep\n" RESET, sid);
                serviceStatus[sid] = 0;
                activeSmen--;
            }

            // Else move the servicable USER to processing queue and service it 
            else {
                printf(BOLD GREEN "Serviceman %ld processing user %d for %d\n" RESET, sid, temp->uid, temp->useTime);
                temp->state = 0b01;
                pthread_mutex_unlock(&temp->lock);
                USER *tt =  random_remove(&list, temp->uid);
                tt->serviceMan = sid;
                
                sorted_insert2(&processing, tt);
                run_task(tt);
                printf(BOLD YELLOW "Serviceman %d finished servicing user %d\n" RESET, tt->serviceMan, tt->uid);
                random_remove(&processing, tt->uid);
                // Deprecated, (status : Resolved): This portion of code may make you see "Probably a glitch in matrix" warning. This is intended behavior as sending a node for removal in locked state will cause deadlock, as random remove is made to be thread safe with mutexes of its own. This warning comes when node is unlocked for a split second, and acquired by other thread before removal.
            }
        }
        else if(serviceStatus[sid] == 0) {
            sleep(1);
        }
        else {
            break;
        }

    }
    printf(BOLD CYAN "Service Counter %ld going to close." RESET, sid);
    return 0;
}

/*  run_task
 *
 *  This function takes in one argument
 *  of type USER *
 *  Thsi funciton returns nothing
 *
 *  This function makes the thread sleep (busy) for specified amount of time
 */
void run_task(USER *user) {
    user->state = 0b01;
    while(user->useTime) {
        sleep(1);
        user->useTime--;
    }
    // set state to terminated
    user->state = 0b00;
}

/*  help_instructions()
 *
 *  This function takes in no arguments
 *  This function returns nothing
 *
 *  This function prints the possible actions on terminal
 */
void help_instructions() {
    printf(CYAN);
    printf("Following options are available\n");
    printf("1. Quick Add User\n");
    printf("2. Add N users\n");
    printf("3. Add serviceman\n");
    printf("4. Display Current Queue\n");
    printf("5. Display Current User(s) under service\n");
    printf("6. Change Maximum number of users\n");
    printf("7. Change Maximum Arrival and/or Use Time ceil value\n");
    printf("8. Print current Default values\n");
    printf("9. Print this info\n");
    printf("0. Exit\n\n");
    printf(RESET);
}

/*  handle_cmd()
 *
 *  This function takes in no arguments
 *  This function returns a void *
 *
 *  This function displays possible options on terminal, scans a user input
 *  and performs appropriate task
 */
void *handle_cmd() {
    int op;
    help_instructions();
    while(1) {
        scanf("%d", &op);
        int currentProcess = get_count(list);
        switch (op) {
            case 1:{ 
                       // Add a USER to list
                       if(currentProcess < maxProcess) {
                           printf(ITALIC YELLOW "User %d joined wait queue\n" RESET, uid);
                           sorted_insert(&list, uid);
                       }else {
                           printf(BOLD RED"Error: Maximum queue limit. Cannot accept more users. Dropping user %d\n" RESET, uid);
                       }
                       uid++;
                       break;
                   }
            case 2:{
                       int newAdd;
                       printf("Enter number of USERS to add\n");
                       scanf("%d", &newAdd);
                       if(currentProcess + newAdd < maxProcess) {
                           init(newAdd);
                           uid += newAdd;
                       }
                       else {
                           printf(BOLD RED "Error: Maximum queue limit, All users may not be serviced. Only accepting till queue is full\n" RESET);
                           while(newAdd && currentProcess < maxProcess) {
                               sorted_insert(&list, uid);
                               printf(ITALIC YELLOW "User %d joined wait queue\n" RESET, uid);
                               uid++;
                               newAdd--;
                               currentProcess = get_count(list);
                           }
                           printf(BOLD RED "%d USERS had to be dropped and turned away\n" RESET, newAdd);
                           uid += newAdd;
                       }
                       break;
                   }
            case 3:{
                       // Add a serviceman
                       if(maxServicemen + 1 <= MAXMAXS) {
                           maxServicemen++;
                       } else {
                           printf(BOLD RED "Error: Max serviceman limit reached. You do not have enough funds to pay them all\n" RESET);
                       }
                       break;
                   }
            case 4:{
                       // Display current queue
                       display(list);
                       break;
                   }
            case 5:{
                       // Display processing queue
                       display(processing);
                       break;
                   }
            case 6:{
                       // Change queue size
                       int newMax;
                       printf("Enter new max queue size\n");
                       scanf("%d", &newMax);
                       if(newMax < maxProcess) {
                           printf(BOLD RED "Error: new max less than previous max. Changes will take effect once queue size is less than new max. New processes wilol not be accepted\n" RESET); 
                           maxProcess = newMax;
                       } else {
                           maxProcess = newMax;
                       }
                       break;
                   }
            case 7:{
                       int oop2;
                       printf("Change what?\n");
                       printf("1. Arrival time Max ceil\n");
                       printf("2. Use time Max ceil\n");
                       printf("3. Both\n");
                       scanf("%d", &oop2);
                       switch (oop2) {
                           case 1:{
                                      // Change arrival time ceil
                                      int newArrMax;
                                      printf(CYAN "Enter new max arrival time\n" RESET);
                                      scanf("%d", &newArrMax);
                                      maxArrivalTime = newArrMax;
                                      break;
                                  }
                           case 2:{
                                      // Change use time ceil
                                      int newUseMax;
                                      printf(CYAN "Enter new max use time\n" RESET);
                                      scanf("%d", &newUseMax);
                                      maxUseTime = newUseMax;
                                      break;
                                  }
                           case 3:{
                                      // Change arrival time ceil
                                      int newArrMax;
                                      printf(CYAN "Enter new max arrival time\n" RESET);
                                      scanf("%d", &newArrMax);
                                      maxArrivalTime = newArrMax;
                                      
                                      // Change use time ceil
                                      int newUseMax;
                                      printf(CYAN "Enter new max use time\n" RESET);
                                      scanf("%d", &newUseMax);
                                      maxUseTime = newUseMax;
                                      break;
                                  }
                           default:{
                                       printf(RED BOLD "ERROR: Wrong input. Returning to main menu\n" RESET);
                                   }
                       }
                   }
            case 8:{
                       // Display current default values
                       printf(CYAN BOLD "Current default values\n" RESET);
                       printf(CYAN "Max queue size           :   %d\n", maxProcess);
                       printf("Number of serviceman     :   %d\n", maxServicemen);
                       printf("Max arrival time ceil    :   %d\n", maxArrivalTime);
                       printf("Max use time             :   %d\n", maxUseTime);
                       printf("Users in queue           :   %d\n", get_count(list));
                       printf("Users being serviced     :   %d\n", get_count(processing));
                       break;
                   }
            case 9:{
                       // Display help instructions 
                       help_instructions();
                       break;
                   }
            case 0:{
                       // Exit the program
                       ex = 1;
                       break;
                   }
            default:{
                        // Default
                        printf(BOLD RED "Wrong input. Select one from the following\n" RESET);
                        help_instructions();
                        break;
                    }
        }

        // Check exit flag
        if(ex){
            break;
        }
    }
    return 0;
}
