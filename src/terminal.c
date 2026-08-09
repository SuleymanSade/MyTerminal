#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>

#include "../include/carr.h"
#include "../include/commands.h"
#include "../include/utils.h"

// `carr` and `carr_list` struct and their functions has been moved to `carr.h` and `carr.c`

void add_history(carr_list* history, carr command);

// command runner(s)
void run_commands(carr_list cmd, carr_list history);

int start();

// Seperated main like this for testing purposes
int main(){
    return start();
}

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
    else if(strcmp(cmd.list[0].arr, "read")==0){
        carr_list fileContent;
        carr_list_alloc(&fileContent, 1024);
        
        read_file(cmd.list[1], &fileContent);

        for(size_t i=0; i<fileContent.used; ++i){
            // Only prints the found ones
            printf("%s", fileContent.list[i].arr);
        }
        carr_list_delete(&fileContent);
        printf("\n");
    }
    else if(strcmp(cmd.list[0].arr, "search") == 0){
        if(cmd.used < 4){
            print_err_all("Too few arguments for command `search`", "too few args", "run_commands()");
            return ;
        }

        carr searchPhrase;
        carr_init(&searchPhrase);
        carr_copy_carr(&searchPhrase, cmd.list[1]);

        if(strcmp(cmd.list[2].arr, "in") != 0){
            print_err_loc("Has to be searched in a file for now, will be changed in future", "run_commands() in search if statement");
            carr_delete(&searchPhrase);
            return ;
        }

        carr_list fileContent;
        carr_list_alloc(&fileContent, 1024);

        read_file(cmd.list[3], &fileContent);

        carr_list foundLines;
        carr_list_alloc(&foundLines, 1024);

        find_phrases(searchPhrase, fileContent, &foundLines);

        for(size_t i=0; i<foundLines.used; ++i){
            printf("%ld) %s", i, foundLines.list[i].arr);
        }
        printf("\n");

        carr_delete(&searchPhrase);
        carr_list_delete(&foundLines);
        carr_list_delete(&fileContent);
    }
    else{
        run_ext_command(cmd);
    }
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