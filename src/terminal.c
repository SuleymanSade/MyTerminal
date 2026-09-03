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

        for(size_t i=0; i < cmd.used; ++i){
            printf("%s\n",cmd.list[i].arr);
        }

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
    bool isQuoteOpen = false, lastSeenBackSlash = false;
    char quoteType = ' '; // " , ', `
    carr_alloc(&buff, 1024);
    int last_div =0;
    for(size_t i = 0; i < text.used; ++i){
        // Seperates by spaces
        if( (!isQuoteOpen  && text.arr[i] == ' ') 
            || (isQuoteOpen && text.arr[i] == quoteType && !lastSeenBackSlash)){
            
            isQuoteOpen = false;

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
            if (!isQuoteOpen 
                && (text.arr[i] == '\'' || text.arr[i] == '"' || text.arr[i] == '`')){
                
                isQuoteOpen = true;
                quoteType = text.arr[i];
                last_div = i+1;
            }
            else{
                // We don't want to include the quote to the buff
                buff.arr[i - last_div] = text.arr[i];
                buff.used+=1;;
            }

            // To allow usage of quotes in strings with the backslash
            if (text.arr[i] == '\\') {
                lastSeenBackSlash = true;
            }
            else{
                lastSeenBackSlash = false;
            }
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