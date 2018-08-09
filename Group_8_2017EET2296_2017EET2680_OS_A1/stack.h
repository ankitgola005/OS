/*  stack.h
 *
 *  Ankit Gola:                 2017EET2296
 *  25.01.2018
 *
 *  This is the header file containing function definitions for 
 *  implementing stack data structure.
 *  This file contains only brief descriptions of functions used.
 *  For detailed function descriptions, check "stack.c".
 *
 */

// Structure for stack
typedef struct node {
    char str[PATH_MAX];
    struct node *next;
} node;

// Function to display a stack data structure
void dispStruct(node *head, char *delim);

// Function to create one node of stack
node *createNode(char *str);

// Function to push node on stack
void pushStruct(node **head, char *str);

// Function to pop from stack
char *popStruct(node **head);

// Function to delete an arbitrary node
void del_anyNode(node **head, char *str);

// Function to return node containing given string
node *stringToNode(FILE *fp);
