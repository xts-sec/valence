//
// Library Dependencies
//
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//
// Define Colour Profiles
// Uncomment as used to keep compiled size down
//
#define RESET   "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
//#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
//#define MAGENTA "\033[35m"      /* Magenta */
//#define CYAN    "\033[36m"      /* Cyan */
//#define WHITE   "\033[37m"      /* White */
//#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
//#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
//#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
//#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
//#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
//#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
//#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
//#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

// define token delimiters
#define TOKEN_DELIM " \t\r\n"
#define DEFAULT_BUFFER_SIZE 1024

// declarations for backend functions
char  * read_line();
char **split_line(char  * );
int valence_execute(char **);

// declarations for builtin shell functions
int valence_exit();
int valence_cd(char **args);
int valence_help();

//  List of builtin command names
char  * builtin_names[] = {
    "help",
    "cd",
    "exit"
};

// list of builtin command functions
int ( * builtin_functions[]) (char **) = {
    &valence_help,
    &valence_cd,
    &valence_exit
};

// calculate the number of builtin command names
int valence_num_builtins() {
    return sizeof(builtin_names)  /  sizeof(char  * );
}

//
// name: valence_help
// desc: display halp info to user, including built in command list
// args: none
int valence_help() {
    fprintf(stdout, GREEN "#################################################\n");
    fprintf(stdout, "# valence shell - made by xts-sec               #\n");
    fprintf(stdout, "# PRE_RELEASE VERSION - WORK IN PROGRESS        #\n");
    fprintf(stdout, "#################################################\n\n" RESET);
    
    fprintf(stdout, "Built in commands:\n\n");
    
    for(int i = 0;
    i < valence_num_builtins();
    i++) {
        printf("  %s\n", builtin_names[i]);
    }

    printf("\nUse the man command for information on other programs.\n\n");
    return 1;
}

//
// name: valence_exit
// desc: cleanly exits the shell by returning 0
// args: character array "args"
//
int valence_exit() {
    return 0;
}

//
// name: valence_cd
// desc: changes the current working directory
// args: character array "args"
int valence_cd(char **args) {
    if (args[1] == NULL)  {
        //fprintf(stderr, RED "valence: argument expected\n" RESET);
        fprintf(stderr, GREEN "valence: cd: no argument provided, going home\n" RESET);
        chdir(getenv("HOME"));
    } else {
        if (chdir(args[1]) != 0)  {
            fprintf(stderr, RED "valence: cd: invalid driectory\n" RESET);
        }

    }

    return 1;
}

//
// name: valence_execute
// desc: forks a new child process and runs a command
// args: character array "args"
//
int valence_execute(char **args) {
    // 
    pid_t cpid;
    int status;
    if (args[0] == NULL)  {
        return 1;
    }

    for (int i = 0; i < valence_num_builtins(); i++) {
    	if (strcmp(args[0], builtin_names[i]) == 0) {
          	return (*builtin_functions[i])(args);
        }
    }
      
    cpid = fork();
    if (cpid == 0)  {
        if (execvp(args[0], args) < 0) 
            printf(RED "valence: command not found: %s\n" RESET, args[0]);
        exit(EXIT_FAILURE);
    } else if (cpid < 0) 
        printf(RED "Error forking" RESET "\n");
    else {
        waitpid(cpid, & status, WUNTRACED);
    }

    return 1;
}

char **split_line(char  *  line) {
    int buffsize = DEFAULT_BUFFER_SIZE;
    int position = 0;
    char **tokens = malloc(buffsize  *  sizeof(char  * ));
    char  * token;
    if (!tokens)  {
        fprintf(stderr, RED "valence: Allocation error\n" RESET);
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIM);
    while (token != NULL)  {
        tokens[position] = token;
        position++;
        if (position >= buffsize)  {
            buffsize += DEFAULT_BUFFER_SIZE;
            tokens = realloc(tokens, buffsize  *  sizeof(char  *  ));
            if (!tokens)  {
                fprintf(stderr, RED "valence: Allocation error\n" RESET);
                exit(EXIT_FAILURE);
            }

        }

        token = strtok(NULL, TOKEN_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

char  * read_line() {
    int buffsize = DEFAULT_BUFFER_SIZE;
    int position = 0;
    char  * buffer = malloc(sizeof(char)  *  buffsize);
    int c;
    if (!buffer)  {
        fprintf(stderr, RED "dash: Allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)  {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r' || c == EOF)  {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;
        if (position >= buffsize)  {
            buffsize += DEFAULT_BUFFER_SIZE;
            buffer = realloc(buffer, buffsize);
            if (!buffer)  {
                fprintf(stderr, RED "valence: Allocation error\n" RESET);
                exit(EXIT_FAILURE);
            }

        }

    }

}

void loop() {
    char  * line;
    char **args;
    int status = 1;
    char  * cwd = malloc(sizeof(char)  *   DEFAULT_BUFFER_SIZE);
    do {
        getcwd(cwd, DEFAULT_BUFFER_SIZE);
        printf("valence: " BLUE "%s " RESET  "» ", cwd);
        line = read_line();
        args = split_line(line);
        status = valence_execute(args);
        free(line);
        free(args);
    }

    while (status);
}

int main() {
    loop();
    return 0;
}
