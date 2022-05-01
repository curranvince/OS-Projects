#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE 8           // max elements in array
#define MAX_NAME_LENGTH 1024   // max string size

#include <semaphore.h>

typedef struct {
    char** array;                   // storage array for c strings
    int top;                        // array index indicating where the top is
    sem_t mutex;                    // 'inner' semaphore  
    sem_t space_avail;              // space semaphore
    sem_t items_avail;              // items semaphore
} array;

int  array_init(array *s);                    // init the array
int  array_put(array *s, char *hostname);     // place element into array
int  array_get(array *s, char **hostname);    // remove element from array
void array_free(array *s);                    // free the array's resources
int array_empty(array *s);

#endif