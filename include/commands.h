#ifndef COMMANDS_DEF
#define COMMANDS_DEF

#include <stddef.h>
#include "../include/carr.h"

typedef struct
{
    carr command;
    carr_list content;
} Command;

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

// COMMAND UTILS
void find_current_loc(carr *c);
void show_history(carr_list history);
void seperate_commands(carr text, carr_list *cmd);
void change_dir(carr new_dir);
void list_content(carr_list* content, carr target_dir, int N);
void change_dir(carr new_dir);
void create_file(carr file_name, bool is_overwrite);
void run_ext_command(carr_list cmd);
void read_file(carr fileName, carr_list* fileContent);
void find_phrases(carr searchPhrase, carr_list fileContent, carr_list* foundLines);


#endif