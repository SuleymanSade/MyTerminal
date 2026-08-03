#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>

#include "carr.h"

// Colors for terminal output, `C_` for color
#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"

// We are using this approach to prevent incompatibility between linux and windows file creation
// In Windows the header and the function usage are different than that of linux
// The reason why we are doing this definition here is because it discards the other statement and not seen by compilar
// In C, compiler creates pathways for both pathways but if either one of those paths are not soundly for the compiler, it throws error
// This way prevents that by disregarding the other pathway
// Checkout the devlog in README.md for more info
#if defined(_WIN32) || defined(_WIN64)
// This part is never seen by a non-windows machine, eg linux and mac
    #include <direct.h>
    #include <windows.h>
    #define create_dir(path) _mkdir(path)

#else
    // This part is never seen by a windows machine
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    // 0777 grants read, write and exec perms
    #define create_dir(path) mkdir(path, 0777)
#endif
    
// `carr` and `carr_list` struct and their functions has been moved to `carr.h` and `carr.c`

// prototypes
void print_err_all(const char msg[], const char type[], const char location[]);
void print_err_loc(const char msg[], const char location[]);
void print_err_msg(const char msg[]);

// command utils
void find_current_loc(carr *c);
void add_history(carr_list* history, carr command);
void seperate_commands(carr text, carr_list *cmd);

// carr *history[100];
int main(){
    printf("Welcome to the my shell\n");
    printf("Type \"exit\" or \"0\" to exit the shell\n");

    carr_list history;
    carr_list_alloc(&history, 100);

    carr text;
    carr_alloc(&text, 1024);

    carr_list cmd;
    carr_list_alloc(&cmd, 1024);

    do{
        carr_list_clear(&cmd);

        carr curr_loc;
        carr_alloc(&curr_loc, 1024);
        find_current_loc(&curr_loc);

        printf(C_BLUE "myshell: %s> " C_RESET, curr_loc.arr);
        carr_delete(&curr_loc);

        // Reads until reaching "\n"
        // To leave space for '\0' needs to input one less than 1024
        scanf("%1023[^\n]", text.arr);
        text.used = strlen(text.arr) + 1; // +1 is for '\0'

        // Cleans the leftover newline from the previous input
        getchar();

        if(text.used == 0) continue;

        add_history(&history, text);
        // LEFT!!!!!!!!!!!!

        seperate_commands(text, &cmd);
        
        // run_commands(cmd);

    } while(strcmp(text.arr, "exit") && strcmp(text.arr, "0"));

    // Clear everything after exit
    carr_list_delete(&history);
    carr_list_delete(&cmd);
    carr_delete(&text);

    return 1;
}

void seperate_commands(carr text, carr_list *cmd){
    // We are using a buffer instead of cmd to directly store the data
    // So that each of our allocation in cmd perfectly matches the needed size
    carr buff;
    carr_alloc(&buff, 1024);
    int last_div =0;
    for(size_t i = 0; i < text.used; ++i){
        // Seperates by spaces
        if(text.arr[i] == ' '){
            // The reason for this additional condition check is to disregard double spaces as a single space 
            // so it doesn't impact the command seperation
            if(buff.used > 0){
                buff.arr[buff.used] = '\0';
                buff.used +=1;

                carr_alloc(&cmd->list[cmd->used], buff.used);
                carr_copy_carr(&cmd->list[cmd->used], buff);
                cmd->used +=1;
                                
                carr_delete(&buff);
                carr_alloc(&buff, 1024);
            }
        }
        else{
            buff.arr[i - last_div] = text.arr[i];
            buff.used+=1;
        }
    }

    if(buff.used > 0){
        buff.arr[buff.used] = '\0';
        buff.used +=1;

        carr_alloc(&cmd->list[cmd->used], buff.used);
        carr_copy_carr(&cmd->list[cmd->used], buff);
        cmd->used +=1;
                        
        carr_delete(&buff);
    }
}

void find_current_loc(carr *c){
    int cap = c->capacity;
    while((getcwd(c->arr, c->capacity) == NULL) && ((c->capacity) < 3e4)){
        // doubles the size of buffer if it doesn't fit the leght of abs_path
        // hard stop at 3e4 as a safety net
        cap*=2;
        carr_delete(c);
        carr_alloc(c, cap);
    }

    c->used = strlen(c->arr);
}

void add_history(carr_list* history, carr command){
    // If the history's cap is not large enough, double it
    if(history->capacity <= history->used){
        history->list = realloc(
            history->list,
            history->capacity*2
        );
        history->capacity*=2;
    }

    carr_alloc(&history->list[history->used], command.used);
    
    carr_copy_carr(&history->list[history->used], command);
    history->used +=1;
}


void show_history(carr_list history){
    for(size_t i=0; i<history.used; ++i){
        // Colorful output
        printf(C_CYAN "%zu)" C_RESET C_GREEN "%s" C_RESET "\n", i, history.list[i].arr);
    }
}