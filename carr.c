#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "carr.h"

/*
All the carr modification/creation functions
*/
void carr_init(carr *c){
    c->arr = NULL;
    c->used = 0;
    c->capacity = 0;
}

void carr_alloc(carr *c, size_t capacity){
    c->capacity = capacity;
    c->used = 0;

    // This works like the new in c++
    // We are allocating space that is the size of a char type times how many chars to allocate
    c->arr = (char*)malloc(c->capacity * sizeof(char));
}

void carr_delete(carr *c){
    if(c->arr != NULL){
        free(c->arr);
    }

    carr_init(c);
}

/*
Copies the content of the c2 to c1
- This operation doesn't impact the capacity of c1.
     unless it is not enough to hold the arr, in which c1's capacity is exactly the size of the passed info
*/
void carr_copy_carr(carr *c1, carr c2){
    if(c2.arr == NULL){
        c1->capacity=0;
        c1->used=0;
        c1->arr=NULL;
        return ;
    }

    if(c1->capacity < c2.used){
        if(c1->arr != NULL){
            carr_delete(c1);
        }
        carr_alloc(c1, c2.used);
    }

    c1->used = c2.used;

    strcpy(c1->arr, c2.arr);
}

void carr_copy_char(carr *c1, const char text[]){
    // +1 is added to account for '\0' at the end of the string
    size_t text_len = strlen(text) + 1;

    if(text_len > c1->capacity){
        carr_delete(c1);
        carr_alloc(c1, text_len);
    }

    strcpy(c1->arr, text);
    c1->used = text_len;
}

/*
All the carr_list modification/creation functions
*/

void carr_list_init(carr_list *cl){
    cl->list = NULL;
    cl->capacity=0;
    cl->used=0;
}

void carr_list_alloc(carr_list *cl, size_t capacity){
    cl->list = (carr*)malloc(sizeof(carr) * capacity);
    cl->capacity = capacity;
    cl->used = 0;
}


// Deletes the contents of of carr_list.list but the list itself stays 
void carr_list_clear(carr_list *cl){
    if(cl->list != NULL){
        for(size_t i=0; i < cl->used; ++i){
            carr_delete(&(cl->list[i]));
        }
    }
    
    cl->used = 0;
}

// Deletes everything in the carr_list
void carr_list_delete(carr_list *cl){
    carr_list_clear(cl);

    if(cl->list != NULL){
        free(cl->list);
    }

    carr_list_init(cl);
}

void carr_list_fill(carr_list *cl, size_t number_carr, size_t carr_cap){
    if(number_carr > cl->capacity){
        carr_list_delete(cl);
        carr_list_alloc(cl, number_carr);
    }

    for(size_t i=0; i<number_carr; ++i){
        carr_alloc(&(cl->list[i]), carr_cap);
    }
    cl->used = number_carr;
}

// The reason cl1 is passed by pointer and cl2 by value
// is because we want to change the contents of cl1 but not touch cl2
void carr_list_copy_carr_list(carr_list *cl1, carr_list cl2){
    int prev_cap = cl1->capacity;
    carr_list_delete(cl1);

    // If the copied list doesn't fit, make it fit with resizing
    // Otherwise turn back to original cap
    if(cl1->capacity < cl2.used){
        carr_list_alloc(cl1, cl2.used);
    }
    else{
        carr_list_alloc(cl1, prev_cap);
    }

    cl1->used = cl2.used;
    for(size_t i=0; i<cl2.used; ++i){
        carr_alloc(&cl1->list[i], cl2.list[i].capacity);
        carr_copy_carr(&cl1->list[i], cl2.list[i]);
    }
}
