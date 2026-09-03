#include <stdio.h>
#include "../include/utils.h"

void print_err_all(const char msg[], const char type[], const char location[]){
    fprintf(stderr, "a %s error occured in %s: %s\n", type, location, msg);
}

void print_err_loc(const char msg[], const char location[]){
    fprintf(stderr, "an error occured in %s: %s\n", location, msg);
}

void print_err_msg(const char msg[]){
    fprintf(stderr, "an error occured in: %s\n", msg);
}