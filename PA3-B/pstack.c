#include "pstack.h"
#include "semaphore.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// init the stack
int stack_init(pstack *s) {
    s->top = 0;					         // init with 0 items
    s->array = (char**)malloc(MAX_INPUT_FILES*sizeof(char*)); // malloc for amount of strings in array 
    for (int i = 0; i < MAX_INPUT_FILES; i++) {               // malloc space for each string
	    s->array[i] = (char*)malloc(1024*sizeof(char));
    }
    sem_init(&s->mutex, 0, 1);                            // init mutex
    return 0;
}

// put str on stack
int stack_put(pstack *s, char *hostname) {
    if (strlen(hostname) > 1024) return -1; // if input too long return error
    sem_wait(&s->mutex); 		            // wait on inner mutex
    strcpy(s->array[s->top++], hostname);   // CRIT SECTION accessing shared array
    sem_post(&s->mutex);		            // release inner mutex
    return 0;				                // return success
}

// pop str from stack
int stack_get(pstack *s, char **hostname) { 
    if (s->top < 1) return -1;				 // return failure if stack is empty      
    sem_wait(&s->mutex);                     // wait for mutex
    if (s->top < 1) {
        sem_post(&s->mutex);                 // check if stack was emptied while we waited
        return -1;
    }                			             
    strcpy(*hostname, s->array[--s->top]);	 // CRIT SECTION accessing shared array, copying to given param
    sem_post(&s->mutex);      			     // free inner mutex
    return 0;						         // return success
}

// free everything we've allocated in init
void stack_free(pstack *s) {
    for (int i = 0; i < MAX_INPUT_FILES; i++) { // free each string
	    free(s->array[i]);
    }
    free(s->array);                       // free array
    sem_destroy(&s->mutex);               // free semaphore
}