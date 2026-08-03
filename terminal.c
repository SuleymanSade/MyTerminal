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
void change_dir(carr new_dir);
void list_content(carr_list* content, carr target_dir, int N);

// command runner(s)
void run_commands(carr_list cmd, carr_list history);

int start();

// Seperated main like this for testing purposes
int main(){
    return start();
}

// carr *history[100];
int start(){
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

        seperate_commands(text, &cmd);
        
        run_commands(cmd, history);

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
                last_div = i+1;
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
        printf(C_CYAN "%zu) " C_RESET C_GREEN "%s" C_RESET "\n", i, history.list[i].arr);
    }
}

void change_dir(carr new_dir){
    // const carr curr = find_current_loc();
    if(strcmp(new_dir.arr, "out") == 0){
        chdir("..");
    }
    else{
        chdir(new_dir.arr);
    }
}

void list_content(carr_list* content, carr target_dir, int N){
    DIR* dir = opendir(target_dir.arr);

    struct dirent* entry;
    
    int i=0;
    while(i<N && (entry = readdir(dir)) != NULL){
        // Hides the hidden files
        if(entry->d_name[0] == '.'){
            i-=1;
        }
        else{
            // printf("%s\n", entry->d_name);
            // carr_alloc(&content->list[i], 1024);
            carr_init(&content->list[i]);
            carr_copy_char(&content->list[i], entry->d_name);
            content->used+=1;
            // strcpy(content[i].lis.arr, entry->d_name);
        }
        i+=1;
    }

    closedir(dir);
}

void create_file(carr file_name, bool is_overwrite){
    FILE *fptr;
    if(is_overwrite){
        fptr = fopen(file_name.arr, "w");
    }
    else{
        fptr = fopen(file_name.arr, "a");
    }

    fclose(fptr);
}

// NOTE that this code is replaced with a macro

void run_ext_command(carr_list cmd){
#if defined(_WIN32) || defined(_WIN64)
    printf("Unknown command, note external bash commands are not supported in windows");

#else
    char* command[cmd.used + 1];
    for(size_t i=0; i<cmd.used; ++i){
        command[i] = cmd.list[i].arr;
    }
    command[cmd.used] = NULL;

    // We create a copy of the current process, which the copy would be terminated by exec
    pid_t pid = fork();
    
    if(pid == 0){
        // Child process

        // Runs the external command
        execvp(command[0], command);
    }
    else{
        // Parent process
        int status;
        // Waits for child process to finish
        waitpid(pid, &status, 0);
    }

    
#endif
}


void run_commands(carr_list cmd, carr_list history){
    if(strcmp(cmd.list[0].arr, "here") == 0){
        carr pos;
        carr_alloc(&pos, 1024);
        find_current_loc(&pos);

        printf("you are at: %s\n", pos.arr);

        carr_delete(&pos);
    }
    else if(strcmp(cmd.list[0].arr, "go") == 0){
        if(cmd.used < 2){
            print_err_all("Too few arguments for command `go`", "missing args", "run_commands()");
        }
        else{
            change_dir(cmd.list[1]);
        }
    }
    else if(strcmp(cmd.list[0].arr, "show") == 0){
        carr target_dir;
        carr_init(&target_dir);

        int N = 100;

        switch (cmd.used)
        {
        case 1:
            // Only shows the default number of files that is stored inside (100)
            // Shows the files in the current directory if not specified
            carr_copy_char(&target_dir, ".");
            break;
        case 2:
            carr_copy_carr(&target_dir, cmd.list[1]);
            break;
        case 3:
            N = atoi(cmd.list[2].arr);
            carr_copy_carr(&target_dir, cmd.list[1]);
            break;
        default:
            print_err_all("Too many arguments for command `show`", "missing args", "run_commands()");
            goto error_label;
            break;
        }

        carr_list content;
        carr_list_alloc(&content, N);

        list_content(&content, target_dir, N);

        for(size_t i=0; i<content.used; ++i){
            printf("%s\n", content.list[i].arr);
        }

        carr_list_delete(&content);
        error_label:
        carr_delete(&target_dir);
    }
    else if(strcmp(cmd.list[0].arr, "create") == 0 || strcmp(cmd.list[0].arr, "cr") == 0){
        if(cmd.used < 3){
            print_err_all("Too few arguments for command `create`/`cr`", "too many args", "run_commands()");
            return ;
        }

        if(strcmp(cmd.list[1].arr, "file") == 0){
            if(cmd.used < 4){
                create_file(cmd.list[2], false);
            }
            else{            
                // lowercase the whole word
                for(size_t i=0; i < cmd.list[3].used; ++i){
                    cmd.list[3].arr[i] = tolower(cmd.list[3].arr[i]);
                }

                bool isOverwrite = strcmp(cmd.list[3].arr, "yes") == 0 || strcmp(cmd.list[3].arr, "y") == 0;
                create_file(cmd.list[2], isOverwrite);
            }
        }
        else if(strcmp(cmd.list[1].arr, "folder") == 0 || strcmp(cmd.list[1].arr, "dir") == 0 || strcmp(cmd.list[1].arr, "directory") == 0){
            create_dir(cmd.list[2].arr);
        }

    }
    else if(strcmp(cmd.list[0].arr, "history")==0){
        show_history(history);
    }
    // else if(strcmp(cmd[0]->arr, "read")==0){
    //     carr fileContent[1024];

    //     for(int i=0; i<1024; ++i){
    //         carr_alloc(&(fileContent[i]), 1024);
    //     }
        
    //     int numLinesRead = read_file(*cmd[1], fileContent);

    //     for(int i=0; i<1024; ++i){
    //         // Only prints the found ones
    //         if(i<numLinesRead)
    //             printf("%s", fileContent[i].arr);

    //         // Clear the whole thing
    //         carr_delete(&fileContent[i]);
    //     }
    //     printf("\n");

        
    // }
    // else if(strcmp(cmd[0]->arr, "search")==0){
    //     carr searchPhrase = {0};
    //     carr_alloc(&searchPhrase, 1024);
    //     carr_copy(&searchPhrase, cmd[1]);

    //     if(strcmp(cmd[2]->arr, "in") != 0){
    //         print_err_loc("Has to be searched in a file for now, will be changed in future", "run_commands() in search if statement");
    //         carr_delete(&searchPhrase);
    //         return ;
    //     }
        
    //     carr fileContent[1024];

    //     for(int i=0; i<1024; ++i){
    //         // carr_init(&fileContent[i]);
    //         carr_alloc(&fileContent[i], 1024);
    //     }

    //     int numLinesRead = read_file(*cmd[3], fileContent);

    //     carr foundLines[1024];
    //     for(int i=0; i<1024; ++i){
    //         // carr_init(&foundLines[i]);
    //         carr_alloc(&foundLines[i], 1024);
    //     }

    //     find_phrases(searchPhrase, fileContent, foundLines, numLinesRead);

    //     for(int i=0; i<1024 && foundLines[i].arr[0] != '\0'; ++i){
    //         printf("%d) %s", i, foundLines[i].arr);
    //     }

    //     carr_delete(&searchPhrase);
    //     for(int i=0; i<1024; ++i){
    //         carr_delete(&fileContent[i]);
    //         carr_delete(&foundLines[i]);
    //     }

    //     printf("\n");
    // }
    else{
        run_ext_command(cmd);
    }
}


void print_err_all(const char msg[], const char type[], const char location[]){
    fprintf(stderr, "a %s error occured in %s: %s\n", type, location, msg);
}

void print_err_loc(const char msg[], const char location[]){
    fprintf(stderr, "an error occured in %s: %s\n", location, msg);
}

void print_err_msg(const char msg[]){
    fprintf(stderr, "an error occured in: %s\n", msg);
}