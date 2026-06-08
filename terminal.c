#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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
        // Reads until reaching "\n"
        // To leave space for '\0' needs to input one less than 1024
        scanf("%1023[^\n]", text->arr);
        // Cleans the leftover newline from the previous input
        getchar();
        
        if(strlen(text->arr) == 0) continue;

        seperate_commands(text, cmd, &n_cmd);

        
        // run_commands(cmd, n_cmd);

        for(int i=0; i<n_cmd; ++i){
            carr_delete(cmd[i]);
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

                // cmd[cmd_row] = new char[1024];
                
                carr_init(cmd[cmd_row]);
                carr_alloc(cmd[cmd_row], 1024);
                n_cmd+=1;
            }
        }
        else{
            cmd[cmd_row][cmd_col] = text[i];
            cmd_col+=1;
        }

        i+=1;
    }

    n_cmd +=1;
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

// void change_dir(carr& new_dir){
//     // const carr curr = find_current_loc();
//     if(strcmp(new_dir.get_arr(), "out") == 0){
//         chdir("..");
//     }
//     else{
//         chdir(new_dir.get_arr());
//     }

//     int i=0;
// }
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

// void list_content(carr content[], carr& target_dir, int N){
//     DIR* dir = opendir(target_dir);

//     // target_dir = (target_dir.get_arr() == nullptr) ? "." : target_dir; 

//     struct dirent* entry;
    
//     int i=0;
//     while(i<N && (entry = readdir(dir)) != nullptr){
        
        
//         // Hides the hidden files
//         if(entry->d_name[0] == '.'){
//             i-=1;
//         }
//         else{
//             content[i] = entry->d_name;
//             char * empty = "\0";
//             strcat(content[i], empty);
//         }
//         i+=1;
//     }

//     closedir(dir);
// }

// bool run_commands(carr cmd[], int n_cmd){
//     if(strcmp(cmd[0], "here") == 0){
//         std::cout << find_current_loc() << "\n";
//     }
//     else if(strcmp(cmd[0], "go") == 0){
//         change_dir(cmd[1]);
//     }
//     else if(strcmp(cmd[0], "show") == 0){
//         const int N = (n_cmd < 2) ? 100: carr_to_int(cmd[2], 100);
//         carr content[N];
//         carr empty_dir(2);
//         empty_dir[0] = '.'; empty_dir[1] = '\0';

//         // list_content(content, (n_cmd < 1) ? empty_dir:cmd[1], N);
//         list_content(content, empty_dir, 100);
//         for(int i=0; i<N && content[i].get_arr() != nullptr; ++i){
//             std::cout << content[i] << "\n";
//         }
//     }
//     return false;
// }

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
}
