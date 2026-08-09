#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>

#include "../include/carr.h"
#include "../include/utils.h"
#include "../include/commands.h"

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

void read_file(carr fileName, carr_list* fileContent){ 
    FILE *fileptr = fopen(fileName.arr, "r");

    
    // No file found
    if(fileptr == NULL){
        print_err_all("undable to find the file", "invalid filename", "read_file()");
        return ;
    }

    // File found

    char* lineInput = (char*)malloc(sizeof(char) * 1024);

    // Loop goes through the file and puts each line to the filecontent arr
    while(fgets(lineInput, 1024, fileptr) != NULL){
        carr_init(&fileContent->list[fileContent->used]);
        carr_copy_char(&fileContent->list[fileContent->used], lineInput);
        fileContent->used+=1;
    }
    free(lineInput);

    fclose(fileptr);
}

void find_phrases(carr searchPhrase, carr_list fileContent, carr_list* foundLines){    
    if(searchPhrase.used == 0){
        print_err_loc("The search phrase is empty (\'\')", "find_phrases()");
        return ;
    }

    for(size_t lineIndex=0; lineIndex<fileContent.used; ++lineIndex){
        bool isFound = false;
        // The upper limit is based on the last point where searchPhrase can start based on its length
        for(size_t i = 0; (i + searchPhrase.used)<(fileContent.list[lineIndex].used) && !isFound; i++){
            // Tries to match searchPhrase from each letter
            // Goes back whenever a mismatch happens
            for(size_t j=0; j<(searchPhrase.used) && fileContent.list[lineIndex].used > (j+i); ++j){
                if(fileContent.list[lineIndex].arr[i+j] != searchPhrase.arr[j]){
                    break;
                }

                // -2 because of extra '\0' at the end of searchPhrase
                if(j == searchPhrase.used-2){
                    isFound = true;
                }
            }
        }

        if(isFound){
            carr_init(&foundLines->list[foundLines->used]);
            carr_copy_carr(&foundLines->list[foundLines->used], fileContent.list[lineIndex]);
            foundLines->used +=1;
        }
    }
}

// ALL THE RUNNER COMMANDS

void run_here(){
    carr pos;
    carr_alloc(&pos, 1024);
    find_current_loc(&pos);

    printf("you are at: %s\n", pos.arr);

    carr_delete(&pos);
}

void run_go(carr_list cmd){
    if(cmd.used < 2){
        print_err_all("Too few arguments for command `go`", "missing args", "run_commands()");
    }
    else{
        change_dir(cmd.list[1]);
    }
}

void run_show(carr_list cmd){
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

void run_create(carr_list cmd){
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

void run_history(carr_list history){
    show_history(history);
}

void run_read(carr_list cmd){
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

void run_search(carr_list cmd){
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

void run_commands(carr_list cmd, carr_list history){
    if(strcmp(cmd.list[0].arr, "here") == 0){
        run_here();
    }
    else if(strcmp(cmd.list[0].arr, "go") == 0){
        run_go(cmd);
    }
    else if(strcmp(cmd.list[0].arr, "show") == 0){
        run_show(cmd);
    }
    else if(strcmp(cmd.list[0].arr, "create") == 0 || strcmp(cmd.list[0].arr, "cr") == 0){
        run_create(cmd);
    }
    else if(strcmp(cmd.list[0].arr, "history")==0){
        run_history(history);
    }
    else if(strcmp(cmd.list[0].arr, "read")==0){
        run_read(cmd);
    }
    else if(strcmp(cmd.list[0].arr, "search") == 0){
        run_search(cmd);
    }
    else{
        run_ext_command(cmd);
    }
}
