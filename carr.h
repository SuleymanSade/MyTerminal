// As a safety to prevent multiple definitions of this file
#ifndef CARR_H
#define CARR_H

#include <stddef.h>

// For convenience this struct stores the size and capacity of the array
// And is standarized
typedef struct
{
    char* arr;
    size_t used; // The number of chars used
    size_t capacity; // The allocated space
} carr;

// This struct will be used to better manage memory by utilizing used
// This way the program will know when to allocate
typedef struct
{
    carr* list;
    size_t used;
    size_t capacity;
} carr_list;

// PROTOTYPES
// carr
void carr_init(carr *c);
void carr_alloc(carr *c, size_t capacity);
void carr_delete(carr *c);
void carr_copy_carr(carr *c1, carr c2);
void carr_copy_char(carr *c1, const char text[]);

// carr_list
void carr_list_init(carr_list *cl);
void carr_list_alloc(carr_list *cl, size_t capacity);
void carr_list_delete(carr_list *cl);
void carr_list_clear(carr_list *cl);
void carr_list_fill(carr_list *cl, size_t number_carr, size_t carr_cap);
void carr_list_copy_carr_list(carr_list *cl1, carr_list cl2);

#endif