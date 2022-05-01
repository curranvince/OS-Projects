#include "array.h"
#include "semaphore.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// init the stack
int array_init(array *s) {
    s->top = 0;					         // init with 0 items
    s->array = (char**)malloc(ARRAY_SIZE*sizeof(char*)); // malloc for amount of strings in array
    for (int i = 0; i < ARRAY_SIZE; i++) {               // malloc space for each string
	s->array[i] = (char*)malloc(MAX_NAME_LENGTH*sizeof(char));
    }
    sem_init(&s->mutex, 0, 1);                            // init semaphores
    sem_init(&s->space_avail, 0, ARRAY_SIZE);
    sem_init(&s->items_avail, 0, 0);
    return 0;
}

// put str on stack
int array_put(array *s, char *hostname) {
    if (strlen(hostname) > MAX_NAME_LENGTH) return -1;
    //if (s->top >= ARRAY_SIZE) return -1;    // return failed if the stack is ful
    sem_wait(&s->space_avail);              // wait on space to be available
    sem_wait(&s->mutex); 		    // wait on inner mutex
    strcpy(s->array[s->top++], hostname);   // CRIT SECTION accessing shared array
    sem_post(&s->mutex);		    // release inner mutex
    sem_post(&s->items_avail);              // signal items available
    printf("%s added to stack\n", hostname);
    return 0;				    // return success
}

// pop str from stack
int array_get(array *s, char **hostname) { 
    //if (s->top < 1) return -1;				     // return failure if stack is empty
    sem_wait(&s->items_avail); 				     // wait for items to be available       
    sem_wait(&s->mutex); 			             // wait for 'inner' or 'accesor' mutex
    *hostname = (char*)malloc(MAX_NAME_LENGTH*sizeof(char)); // malloc space for hostname
    strcpy(*hostname, s->array[--s->top]);	  	     // CRIT SECTION accessing shared array
    sem_post(&s->mutex);      				     // free inner mutex
    sem_post(&s->space_avail);				     // signal space is available
    printf("removed %s from stack\n", *hostname);
    return 0;						     // return success
}

// free everything we've allocated in init
void array_free(array *s) {
    for (int i = 0; i < ARRAY_SIZE; i++) { // free each string
	free(s->array[i]);
    }
    free(s->array);                       // free array
    sem_destroy(&s->mutex);               // free semaphores
    sem_destroy(&s->space_avail);
    sem_destroy(&s->items_avail);
}
