#ifndef PSTACK_H
#define PSTACK_H

#include <semaphore.h>

#define MAX_INPUT_FILES 100

typedef struct {
    char** array;                   // storage array for c strings
    int top;                        // array index indicating where the top is
    sem_t mutex;                    // 'inner' semaphore  
} pstack;

int  stack_init(pstack *s);                    // init the array
int  stack_put(pstack *s, char *hostname);     // place element into array
int  stack_get(pstack *s, char **hostname);    // remove element from array
void stack_free(pstack *s);                    // free the array's resources

#endif