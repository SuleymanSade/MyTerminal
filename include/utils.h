#ifndef UTILS_DEF
#define UTILS_DEF

// prototypes
void print_err_all(const char msg[], const char type[], const char location[]);
void print_err_loc(const char msg[], const char location[]);
void print_err_msg(const char msg[]);

#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"

#endif