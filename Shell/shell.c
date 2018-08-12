/*  shell.c
 *
 *  Ankit Gola:                 2017EET2296
 *
 *  28.01.2018
 *
 *  This is the function file for "shell_main.c"
 *  It contains the function definitions called in main()
 *  Function descriptions, including number of arguments, 
 *  type of arguments, and return type are 
 *  mentioned in comments at appropriate places.
 *  For brief descriptions of these functions, 
 *  refer to "shell.h" header file.
 * 
 */

// Include files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>

// Header file
#include "shell.h"
#include "stack.h"

// Defines
#define RESET "\x1b[0m"
#define BLUE "\x1b[34m"
#define GREEN "\x1b[32m"
#define BOLD "\x1b[1m"

#define DELIM " \t\n\r\a"
#define BUFSIZE 128

// Global Variables
node *dirs = NULL;
node *path = NULL;

static sigjmp_buf env;
static volatile sig_atomic_t jumpActive = 0;

/*  sh_loop()
 *
 *  This function takes no arguments.
 *  It returns "0" upon successful completion.
 *  
 *  This function loops indefinately till it is either,
 *  1. terminated by user using "ctrl+c"/"ctrl+d"
 *  2. Exited using "exit"
 *
 *  This function provides basic shell interface.
 *  It reads user input from terminal as a string,
 *  parses the string to get command and its arguments,
 *  creates a child process with appropriate arguments,
 *  waits for child to finish,
 *  and loops over the process indefinately, till it is terminated.
 *
 */
int sh_loop() {
    char *input;            // Hold input string
    char **args;            // Hold arguments as array of strings 
    int status = 1;
/*  TODO: Uncomment before submission
    if(set_path()){
        fprintf(stderr, "Cannot reset PATH\n");
        exit(EXIT_FAILURE);
    }
*/
    // Initialise and set dirs stack
    //char dirArr[PATH_MAX];
    //if(!sh_setDirs(dirArr)) {
    //    fprintf(stderr, "Cannot set dirs stack\n");
    //    exit(EXIT_FAILURE);
    //}
    //dirs = createNode(dirArr);
//    dispStruct(dirs, " ");

    // Get current username
    char *uname = getenv("USER");

    // Get hostname
    char hname[1024];
    gethostname(hname, 1024);


//    system("gnome-terminal");  optional, open shell in new terminal
    do {
        // Handle ctrl+c, soft reset the while loop
        if(sigsetjmp(env, 1) == 42) {
            printf("\n");
        }
        jumpActive = 1;
    
        // Get pwd
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));


        printf(BOLD GREEN "%s@%s" RESET ":" BOLD BLUE "%s$ " RESET, uname, hname, cwd);
        
        // Call a function to read user input from terminal till endline "\n"
        input = sh_readInput();

        // If ctrl+D is pressed, Exit shell
        if(!strcmp(input, "ctrld")) {
            printf("\n");
            return 0;
        }

        // Call a funtion to parse user input into arguments
        args = sh_parseLine(input);
//        printf("%s. %s. %s.\n", args[0], args[1], args[2]);
//        status = sh_procCreate(args);
        status = sh_call(args);
//        printf("%s\n", line);     

        // Cleanup
        free(input);
        free(args);
    } while(status);        // Loop while status is not 0
    
    return 0;
}

/*  sh_readInput()
 *
 *  This function takes no arguments.
 *  This funciton returns a string upon successful completion.
 *
 *  This function reads the user input from terminal.
 *  The length of string is determined automatically 
 *  by "getline()" depending on the user input.
 *  
 */ 
char *sh_readInput() {
    char *inputArgs = NULL;                   // char Array to store the input string
    size_t bufsize = 0;                       // getline() defines the buffer size
    int nr = getline(&inputArgs, &bufsize, stdin);     // call getline to get user input
//    printf("result = %zd, bufsize = %zu, line = \"%s\"\n", result, bufsize, intArgs);
   
    // If ctrl+D is pressed, set args to exit shell
    if(nr < 0) {
        strcpy(inputArgs, "ctrld");
    }

    return inputArgs;                         // Return char array read by getline()
}

/*  sh_parseLine()
 *
 *  This function takes single argument.
 *  Argument is of type string.
 *  This function returns an array of strings upon successful completion.
 *
 *  This function parses the string passed to it.
 *  It reads the string and depending on the delimiters,
 *  defined in "DELIM", generates an array of strings
 *  it finds a "NULL", i.e. End of String.
 *
 */
char **sh_parseLine(char *line){
    int bufSize = BUFSIZE, index = 0;
    
    // Allocate memory for arguments
    char **argArr = (char **)malloc(sizeof(char*) * bufSize);
    // Exit if malloc fails to allocate memory
    if(!argArr){
        fprintf(stderr, "sh: malloc error\n");
        exit(EXIT_FAILURE);
    }

    char *arg;
    // Parse string on basis of delimiters defined in "DELIM"
    arg = strtok(line, DELIM);
    // Repeat till End of String/NULL
    while(arg != NULL) {
        argArr[index] = arg;            // Save args in array
        index++;
        
        // If index exceeds pre-allocated buffer size,
        // reallocate a bigger buffer size
        if(index >= bufSize) {
            bufSize += BUFSIZE;
            argArr = realloc(argArr, bufSize * sizeof(char*));
            
            // Exit with error if realloc fails
            if(!argArr) {
                fprintf(stderr, "sh: malloc error\n");
                exit(EXIT_FAILURE);
            }
        }
        arg = strtok(NULL, DELIM); 
    }
    // Set last element of array as NULL
    argArr[index] = NULL;
    
    return argArr;
}

/*  sh_procCreate()
 *
 *  This function takes single argument.
 *  Argument is of type array of strings.
 *  This function returns "1" upon successful execution.
 *
 *  This function calls "fork()" to start a new process.
 *  Upon successful fork(), it calls "execvp()" with arguments provided
 *  to launch the instance of a program.
 *  Parent process waits till completion of child process.
 */
int sh_procCreate(char **args) {
    pid_t pid;
//    int status;

    pid = fork();
    if(pid < 0) {
        // If fork() unsuccessful, print an error
        fprintf(stderr, "Error creating fork()\n");
        return -1;
    } else if(pid == 0) {
        // Child Process execv()
        signal(SIGINT, SIG_DFL);
        printf("I'm child, pid: %d\n", pid);
        if(execvp(args[0], args) == -1) {
            fprintf(stderr, "sh: failed to launch execvp()\n");
        }
        return 0;
    } else {
        // Parent process, wait till child terminates
        waitpid(-1, NULL, 0);            // Wait for all child processes
/*        do {
        //    wpid = waitpid(pid, &status, WUNTRACED);    // Wait till child has stopped
       //     waitpid(pid, &status, WUNTRACED);    // Wait till child has stopped
 //       } while (!WIFEXITED(status) && !WIFSIGNALED(status));   // Terminated normally or by a signal
*/
        printf("I'm parent. pid: %d\n", pid);
    }
    return 1;
}

/*  *builtinArr[]
 *
 *  This is an array of strings.
 *  This array holds the names of builtin functions 
 *  available in the shell.
 */
char *builtinArr[] = {
                        "cd",
                        "exit",
                        "pushd", 
                        "popd", 
                        "dirs",
                        "path"};

/*  sh_builtinCount()
 *
 *  This function takes no arguments.
 *  This function returns an integer upon successful completion.
 *
 *  This function takes the size of builtinArr and returns the
 *  number of builtin functions available in the shell.
 */
int sh_builtinCount() {
    int size = (sizeof(builtinArr)/sizeof(char *));
    return size;
}

/*  (*builtin[])()
 *
 *  This function pointer takes single argument.
 *  Argument type is an array of strings.
 *  This function pointer returns an integer upon successful completion.
 *  The value of integer depends on the function pointed to at.
 *
 *  This is an array of function pointers.
 *  It is used as a LUT to point to a particular function.
 */
int (*builtin[]) (char **args) = { 
    &sh_cd,
    &sh_exit,
    &sh_pushd,
    &sh_popd,
    &sh_dirs,
    &sh_path,
};

/*  sh_cd()
 *
 *  This function takes single argument.
 *  Argument type is an array of strings.
 *  This function returns "1" upon successful compilation.
 *
 *  This function calls chdir() with appropriate arguments,
 *  (path to a location), and switches directory accordingly.
 *
 */
int sh_cd (char **args) {
    if(args[1] == NULL) {       // If no path specified
        fprintf(stderr, "sh: no path specified\n");
        return 0;
    } else {
        if(chdir(args[1])) {    // If path is specified
            fprintf(stderr, "sh: cd failure\n");
        }
    }
    return 1;
}

/*  sh_exit()
 *  
 *  This function takes single argument.
 *  Argument type is an array of strings.
 *  This function returns "0" ipon successful completion.
 *
 *  This function returns 0 and finishes execution.
 *
 */
int sh_exit(char **args) {
    return 0;
}

/*  sh_call()
 *
 *  This function takes a single argument.
 *  Argument type is an array of strings.
 *  This function returns an integer upon successful completion.
 *
 *  This function checks for builtin functions.
 *  If they exist, execute them. 
 *  If they don't, it creates a new process with appropriate arguments.
 *
 */
int sh_call(char **args) {
    if(!args[0]) {
        return 1;       // If no function name passed, return 1
    }

    // Search for called funtion in list of builtin funcitons
    // and call the particular function
    for(int i = 0; i < sh_builtinCount(); i++) {
        if(strcmp(args[0], builtinArr[i]) == 0) {
            return (*builtin[i])(args);
        }
    }

    // If funtion is not builtin, call for function from PATH
    return sh_procCreate(args);
}

/*  sh_setDirs()
 *
 *  This function takes a single argument.
 *  Argument of type string.
 *  This funciton returns "1" upon successful completion.
 *
 *  This function initialises the value of dirs stack with cwd.
 *
 */
int sh_setDirs(char *dirs){
    getcwd(dirs, PATH_MAX);
    return 1;
}

/*  sh_pushd()
 *
 *  This function takes a single argument.
 *  Argument is of type array of string.
 *  This funciton returns "1" upon successful completion.
 *
 *  This function takes in the path, pushes it to dirs stack
 *  and switches to that path directory.
 *
 */
int sh_pushd(char **args) {
    // Initialise dirs
    if(dirs == NULL) {
        char dirArr[PATH_MAX];
        // If cannot get cwd, return error
        if(!sh_setDirs(dirArr)) {
            fprintf(stderr, "Cannot set dirs stack\n");
            exit(EXIT_FAILURE);
        }
        // Creare first node of stack with cwd
        dirs = createNode(dirArr);
    }

    // If cannot change directory, for instance if destn directory doesn't exist, do not update dirs
    if(!sh_cd(args)){
        printf("Dirs not updated\n");
        return 1;
    }
    
    // Get new cwd after changing directory
    char dir_destn[PATH_MAX];
    getcwd(dir_destn, PATH_MAX);

    // Push cwd to stack
    pushStruct(&dirs, dir_destn);
    // Print stack
    sh_dirs(args);

    return 1;
}

/*  sh_popd()
 *
 *  This function takes a single argument.
 *  Argument type is an array of string.
 *  This funciton returns "1" upon successful completion.
 *
 *  This function takes in the dirs stack, pops the path 
 *  from stack, and changes cwd to the path popped.
 *
 */
int sh_popd(char **args) {
    // Pop from stack
    char *popPath = popStruct(&dirs);
    // If pop is successful
    if(popPath) {   
        printf("popPath: %s\n", popPath);       // Print popped dir
        chdir(popPath);                         // Move to popped dir
    } else {
        // Else print error
        fprintf(stderr, "sh: popd error\n");
    }
    return 1;
}

/*  sh_dirs()
 *
 *  This function takes a single argument.
 *  Argument type is an array of string.
 *  This funciton returns "1" upon successful completion.
 *
 *  This function takes in the dirs stack and prints it.
 */
int sh_dirs(char **args) {
    // If dirs is null(not initialised), print cwd
    if(dirs == NULL) {
        char cwd[PATH_MAX];
        printf("%s\n", getcwd(cwd, PATH_MAX));
    } else {
        // Else display struct
        dispStruct(dirs, " ");
    }
    return 1;
}

/*  set_path()
 *
 *  This function takes no arguments
 *  This function return "0" upon successful completion
 *
 *  This function sets the "PATH" variable to "".
 *  This change is temporary and lost as soon as terminal is closed.
 *  To make the change permanent, edit the ".bashrc" file.
 */
int set_path() {
    setenv("PATH", "", 1);          // Set PATH variable to ""
    return 0;
}

/*  sh_path()
 *
 *  This function takes a single argument
 *  Argument type is an array of string.
 *  This function returns 1 upon successful completion.
 *
 *  This function modifies the PATH variable.
 *  It reads the PATH from a file, if it exists. Default is "".
 *  USAGE:
 *      1. path : returns PATH
 *      2. path + /some/path : adds /some/path to PATH
 *      3. path - /some/path : removes /some/path from PATH, if it is present in PATH.
 *
 *  After the modifications, it writes back path into same file, replacing previous PATH.
 *
 */
int sh_path(char **args) {
    // Open PATH file
    FILE *fp = fopen(".shrc", "r");
    node *head = NULL;                      // Head for path

    head = stringToNode(fp);                // Populate PATH list from file
    // If no arguments are passed, display PATH
    if(!args[1]) {
        if(head == NULL) {                  // If PATH is NULL, print ""
            printf("\n");
        }else { 
    	    dispStruct(head, ":");          // Else print PATH
        }
	} else if(!strcmp(args[1], "+") && args[2]){
        // If atleast two arguments exist, modifier and path to be added
        int i = 2;
        //printf("old path :\n");
        //dispStruct(head, "\n");

        // Recursively add all paths in args, if it is a valid directory
        while(args[i]) {
            DIR *dir = opendir(args[i]);        // Check if path is a valid directory 
            if(dir) {                           // If it is, add it to PATH
                pushStruct(&head, args[i]);
                dispStruct(head, ":");          // Display new PATH
            } else if(ENOENT == errno) {        // If Directory does not exist
                fprintf(stderr, "PATH Error: \"%s\" directory does not exist\n", args[i]);
            } else {                            // Default, for all other cases
                fprintf(stderr, "PATH Error: \"%s\" path is not a valid directory\n", args[i]);
            }
            i++;
        }
        //printf("old path :\n");
        //dispStruct(head, "\n");
    } else if(!strcmp(args[1],"-") && args[2]){
        // If atleast two arguments exist, modifier and path to be deleted
        int i = 2;
        //printf("Old Path:\n");
        //dispStruct(head, "\n");
        while(args[i]){                     // Remove path. Condition for checking whether path is present 
            del_anyNode(&head, args[i]);    // in PATH is included in function del_anyNode()
            i++;
        }
        //printf("New Path:\n");
        if(head){                           // If PATH is not NULL, display new PATH
            dispStruct(head, ":");
        } else {                            // If new PATH is now NULL, display ""
            printf("\n");
        }
    } else {
        // Incorrect usage
        fprintf(stderr, "PATH error: incorrect argument passed\n");
        fprintf(stderr, "USAGE:\n\t./env          : Prints PATH\n\t./env + <path> : Add <path> to PATH\n\t./env - <path> : Remove <path> from PATH\n");
    }
    
    fclose(fp);
    
    // Convert the PATH data structure to a string
    int len = 5*PATH_MAX;
    char strTemp[len];
    
    // If PATH is not NULL
    if(head) {
        node *temp = head;
        int count = 0;
        strcpy(strTemp, temp->str);         // Copy head to string
        strcat(strTemp, ":");
        temp = temp->next;
        while(temp != NULL) {
            count += strlen(temp->str);     // Check for string overflow
           // if(count+2 > len){
    
           // }
    
            strcat(strTemp, temp->str);     // Concatenate rest of PATH to string
            //printf("count: %d\tstruct: %s\n", count, temp->str);
            if(temp->next)
                strcat(strTemp, ":");
            temp = temp->next;
        }
        free(temp);
    } else {
        // If PATH is NULL
        strcpy(strTemp, "");
    }
    
   // printf("PATH: %s END", strTemp); 
    // Set enviornment for PATH as stored in string
    setenv("PATH", strTemp, 1);   

    // Save path back in file
    fp = fopen(".shrc", "w");

    // If PATH is not NULL
    if(head != NULL){
        node *temp = head;
        while(temp != NULL) {
            fputs(temp->str, fp);
            //if(temp->next)
            fputs(":", fp);
            temp = temp->next;
        }
        free(temp);
    } else {
        // If PATH is NULL
        fputs("", fp);
    }

    fclose(fp);                 // Close file
    free(head);                 // Cleanup
    return 1;
}


void INThandler(int signo) {
    if(!jumpActive) {
        return;
    }
    longjmp(env, 42);
}
