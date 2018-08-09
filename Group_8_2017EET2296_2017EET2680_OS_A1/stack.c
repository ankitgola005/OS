/*  stack.c
 *
 *  Ankit Gola:                 2017EET2296
 *  25.01.2018
 *
 *  This is the file containing function definitions for implementing
 *  the stack. Stack is used in implementing "pushd()", "popd()", and 
 *  "dirs()" functions.
 *  Further stack is also used in implementing PATH variable.
 *  This file contains detailed description of functions at appropriate
 *  places. For brief description, check "stack.h".
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "stack.h"

/*  dispStruct()
 *
 *  This function takes two arguments.
 *  Arguments are of type pointer to a node, and a string.
 *  This funciton returns nothing
 *
 *  This function takes in the pointer to head node, delimiter between nodes,
 *  and prints the whole stack
 *
 */
void dispStruct(node *head, char *delim) {
    node *p;
    for(p = head; p != NULL; p = p->next) {
        printf("%s%s", p->str, delim);
    }
    printf("\n");
}

/*  createNode()
 *
 *  This function takes a single argument.
 *  Argument is of type string.
 *  This function returns pointer to node.
 *
 *  This function takes in a string, makes a struct of type "node",
 *  and returns a pointer to the node.
 *
 */
node *createNode(char *str) {
    node *new = (node *)malloc(sizeof(node));
    if(!new) {
        fprintf(stderr, "stack: cannot malloc\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new->str, str);
    new->next = NULL;

    return new;
}

/*  pushStruct()
 *
 *  This function takes in two arguments.
 *  Argument type is pointer to pointer to head of stack, 
 *  and a string as arguments.
 *  This function returns nothing
 *
 *  This functions takes a string and pushes it to 
 *  top of the stack, updating the head.
 *
 */
void pushStruct(node **head, char *str) {
    node *new = createNode(str);
    new->next = (*head);
    (*head) = new;
}

/*  popStruct()
 *
 *  This function takes in single argument.
 *  Argumetn type is pointer to pointer to head of stack.
 *  This function returns the string stored in head of stack.
 *
 *  This function takes a stack and pops the head, updating the head.
 *
 */
char *popStruct(node **head) {
    node *temp = *head;
    if(temp == NULL) {
        fprintf(stderr, "Stack error: Stack empty, nothing to pop\n");
        return NULL;
    }

    if(temp->next) {
        char *str = temp->next->str;
        *head = temp->next;
        free(temp);
        return str;
    } else {
        printf("dirs stack empty. Nothing to pop\n");
        return NULL;
    }
    return NULL;
}

/*  delNode()
 *
 */
void del_anyNode(node **head, char *str) {
    node *temp = *head;
    node *prev;
   // dispStruct(*head, "    "); 
    // If list is empty
    if(temp == NULL) {
        fprintf(stderr, "Stack arb error: Stack empty, nothing to delete\n");
        return;
    }

    // If head itself is node to be deleted
    if(!strcmp(temp->str, str)) {
        if(temp->next) {
            *head = temp->next;
            free(temp);
        } else {
            *head = NULL;
            return;
        }
        return;
    }

    // Search for key to be deleted, keep track of previous node
    while(strcmp(temp->str, str)) {
        prev = temp;
        temp = temp->next;
    }

    // If str not present in list
    if(temp == NULL) {
        fprintf(stderr, "Stack error: str not in path\n");
        return;
    }
    
    // Link previous node to next node of current node
    prev->next = temp->next;

    // free memory
    free(temp);
    
    return;
}

/*  stringToNode()
 *
 * This function takes a single argument.
 * Argument is of type FILE pointer.
 * This function returns a pointer to a struct node
 *
 * This functions reads a string from file, parse it,
 * and returns a node of it.
 *
 */
node *stringToNode(FILE *fp) {
    char tempPath[PATH_MAX];
    int i = 0;
    char ch = fgetc(fp);
    node *head = NULL;

    while(ch != EOF) {
        tempPath[i] = ch;
        i++;
        ch = fgetc(fp);

        if(ch == ':'){
            ch = fgetc(fp);
            tempPath[i] = '\0';
            pushStruct(&head, tempPath);
            i = 0;
        }
    }
    return head;
}
