#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>


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
    #define waitpid(pid, status, num) \
        DWORD desAcc = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ; \
        HANDLE h_process = OpenProcess(desAcc, FALSE, pid); \
        WaitForSingleObject(h_process, INFINITE); \
        CloseHandle(h_process) \

#else
    // This part is never seen by a windows machine
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    // 0777 grants read, write and exec perms
    #define create_dir(path) mkdir(path, 0777)
#endif
    

// For convenience this struct stores the size of the array
// And is standarized
typedef struct
{
    char *arr;
    int n;
} carr;

// prototypes
void print_err(const char msg[]);
void carr_init(carr *c);
void carr_alloc(carr *c, int n);
void carr_delete(carr *c);
void carr_copy(carr *c1, carr *c2);
void find_current_loc(carr *c);
int seperate_commands(carr *text, carr *cmd[], int *n_cmd);
void run_commands(carr* cmd[], int n_cmd);


int main(){
    printf("Welcome to the terminal\n");
    printf("Type \"exit\" or \"0\" to exit the terminal\n");
    carr* text = (carr*)malloc(sizeof(carr));
    carr *cmd[1024];
    int n_cmd=0;

    carr_init(text);
    carr_alloc(text, 1024);
    
    
    do{
        cmd[0] = (carr*)malloc(sizeof(carr));
        carr_init(cmd[0]);
        carr_alloc(cmd[0], 1024);

        n_cmd = 0;

        carr* curr = (carr*)malloc(sizeof(carr));
        carr_init(curr);
        carr_alloc(curr, 1024);

        find_current_loc(curr);
        
        printf("coolshell: %s> ", (curr->arr));
        carr_delete(curr);
        free(curr);

        // Reads until reaching "\n"
        // To leave space for '\0' needs to input one less than 1024
        scanf("%1023[^\n]", text->arr);
        // Cleans the leftover newline from the previous input
        getchar();

        if(strlen(text->arr) == 0) continue;

        seperate_commands(text, cmd, &n_cmd);
        
        run_commands(cmd, n_cmd);

        for(int i=0; i<n_cmd; ++i){
            // Deletes the array
            carr_delete(cmd[i]);
            // Deletes the memory address of the carr object itself
            free(cmd[i]);
        }

    } while(strcmp(text->arr, "exit") && strcmp(text->arr, "0"));

    return 0;
}

/*
* Returns: bool
    * true: successfully seperated
    * false: unsuccessful operation
*/
int seperate_commands(carr *text, carr *cmd[], int *n_cmd){
    int i=0, cmd_row=0, cmd_col=0;
    while(text->arr[i] != '\0' && i<1024){
        // Error checks for safety
        if(cmd_col >= 1024){
            print_err("the lenght of a single command exceeds 1024 chars");
            return 0;
        }
        if(cmd_row >= 1024){
            print_err("the lenght of number of commands exceeds 1024 chars"); 
            return 0;
        }
        if(i >= 1024){
            print_err("the lenght of the entire line exceeds 1024 chars, or missing '\\0'");
            return 0;
        }
        
        // Seperates by spaces
        if(text->arr[i] == ' '){
            // The reason for this additional condition check is to disregard double spaces as a single space 
            // so it doesn't impact the command seperation
            if(cmd_col != 0){
                cmd[cmd_row]->arr[cmd_col] = '\0'; // End of a char array 
                cmd_row +=1;
                cmd_col = 0;

                // Need to allocate memory address for the carr* 
                cmd[cmd_row] = (carr*)malloc(sizeof(carr));

                carr_init(cmd[cmd_row]);
                carr_alloc(cmd[cmd_row], 1024);
                *n_cmd+=1;
            }
        }
        else{
            cmd[cmd_row]->arr[cmd_col] = text->arr[i];
            cmd_col+=1;
        }

        i+=1;
    }

    (*n_cmd) +=1;
    if(cmd_col > 0){
        cmd[cmd_row]->arr[cmd_col] = '\0'; // End of a char array 
    }


    return 1;
}

void find_current_loc(carr *c){
    int n = c->n;
    while((getcwd(c->arr, c->n) == NULL) && ((c->n) < 3e4)){
        // doubles the size of buffer if it doesn't fit the leght of abs_path
        // hard stop at 3e4 as a safety net
        n*=2;
        carr_delete(c);
        carr_alloc(c, n);
    }   
}

void change_dir(carr* new_dir){
    // const carr curr = find_current_loc();
    if(strcmp(new_dir->arr, "out") == 0){
        chdir("..");
    }
    else{
        chdir(new_dir->arr);
    }

    int i=0;
}

// int carr_to_int(carr &carr_var, int def){
//     int res = 0, size = 0;

//     while(carr_var[size] != '\0') size += 1;

//     for(int i=0; i<size; ++i){
//         if(carr_var[i] <= '9' && carr_var[i] >= '0'){
//             res += (carr_var[i] - '0') * (size - i);
//         }
//         else{
//             print_err("wrong type", "entered a non-numerical value");
//             return def;
//         }
//     }

//     return res;
// }

void list_content(carr content[], carr target_dir, int N){
    DIR* dir = opendir(target_dir.arr);

    // target_dir = (target_dir.get_arr() == nullptr) ? "." : target_dir; 

    struct dirent* entry;
    
    int i=0;
    while(i<N && (entry = readdir(dir)) != NULL){
        // Hides the hidden files
        if(entry->d_name[0] == '.'){
            i-=1;
        }
        else{
            printf("%s\n", entry->d_name);
        }
        i+=1;
    }

    closedir(dir);
}

void create_file(char* file_name, bool is_overwrite){
    FILE *fptr;
    if(is_overwrite){
        fptr = fopen(file_name, "w");
    }
    else{
        fptr = fopen(file_name, "a");
    }

    fclose(fptr);
}

void run_ext_command(carr* cmd[], int n_cmd){
    char* command[n_cmd+1];
    for(int i=0; i<n_cmd; ++i){
        command[i] = cmd[i]->arr;
    }
    command[n_cmd] = NULL;

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

}

void run_commands(carr* cmd[], int n_cmd){
    if(strcmp(cmd[0]->arr, "here") == 0){
        carr* pos = (carr*)malloc(sizeof(carr));
        carr_init(pos);
        carr_alloc(pos, 1024);
        find_current_loc(pos);

        printf("you are at: %s\n", pos->arr);

        carr_delete(pos);
        free(pos);
    }
    else if(strcmp(cmd[0]->arr, "go") == 0){
        change_dir(cmd[1]);
    }
    else if(strcmp(cmd[0]->arr, "show") == 0){
        carr target_dir;
        carr_init(&target_dir);

        int N =100;

        if(n_cmd == 1){
            // Only shows the default number of files that is stored inside (100)
            carr_alloc(&target_dir, 1024);
            // Shows the files in the current directory if not specified
            strcpy(target_dir.arr, ".");
        }
        else if(n_cmd == 2){
            carr_alloc(&target_dir, cmd[1]->n);
            carr_copy(&target_dir, cmd[1]);
        }
        else if(n_cmd == 3){
            N = atoi(cmd[2]->arr);
            carr_alloc(&target_dir, cmd[1]->n);
            carr_copy(&target_dir, cmd[1]);
        }

        carr content[1024];

        list_content(content, target_dir, N);
        carr_delete(&target_dir);
    }
    else if(strcmp(cmd[0]->arr, "create") == 0 || strcmp(cmd[0]->arr, "cr") == 0){
        if(n_cmd < 3){
            printf("missing number of parameters for file/dir creation\n");
            return ;
        }

        if(strcmp(cmd[1]->arr, "file") == 0){
            if(n_cmd < 4){
                create_file(cmd[2]->arr, false);
            }
            else{
                bool isOverwrite = false;
                // lowercase the whole word
                for(int i=0; i<cmd[3]->n; ++i){
                    cmd[3]->arr[i] = tolower(cmd[3]->arr[i]);
                }
                if(strcmp(cmd[3]->arr, "yes") == 0 || strcmp(cmd[3]->arr, "y") == 0){
                    create_file(cmd[2]->arr, true);
                }
                else{
                    create_file(cmd[2]->arr, false);
                }
            }
        }
        else if(strcmp(cmd[1]->arr, "folder") == 0 || strcmp(cmd[1]->arr, "dir") == 0 || strcmp(cmd[1]->arr, "directory") == 0){
            create_dir(cmd[2]->arr);
        }

    }
    else{
        run_ext_command(cmd, n_cmd);
    }
}

void print_err(const char msg[]){
    fprintf(stderr, "%s", msg);
}

/*
All the carr modification/creation functions
*/
void carr_init(carr *c){
    c->arr = NULL;
    c->n = 0;
}

void carr_alloc(carr *c, int n){
    c->n = n;

    // This works like the new in c++
    // We are allocating space that is the size of a char type times how many chars to allocate
    c->arr = (char*)malloc(c->n * sizeof(char));
}

void carr_delete(carr *c){
    c->n =0;
    if(c->arr != NULL){
        free(c->arr);
    }
}

void carr_copy(carr *c1, carr *c2){
    if(c1->n < c2->n){
        // TODO: WIP
    }
    c1->n = 0;

    if(c2->arr == NULL){
        c1->n=0;
    }

    strcpy(c1->arr, c2->arr);
}
