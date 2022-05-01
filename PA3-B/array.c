#include "array.h"
#include "semaphore.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

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
    sem_wait(&s->space_avail);              // wait on space to be available
    sem_wait(&s->mutex); 		            // wait on inner mutex
    strcpy(s->array[s->top++], hostname);   // CRIT SECTION accessing shared array
    sem_post(&s->mutex);		            // release inner mutex
    sem_post(&s->items_avail);              // signal items available
    return 0;				                // return success
}

// pop str from stack
int array_get(array *s, char **hostname) { 
    if (s->top < 1) return -1;				     // return failure if stack is empty
    int r;
    struct timespec ts;                         // getting current time
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 10;
    // wait for items to be available
    // check after 10 ns if we should exit (since the thread handles calling again)
    while ((r = sem_timedwait(&s->items_avail, &ts) == -1) && (errno == EINTR))
        continue;
    if (r != 0)
        return -1;				                 // exit if we did not acquire access due to lock being free
    sem_wait(&s->mutex); 			             // wait for 'inner' or 'accesor' mutex
    if (s->top < 1) {                            // exit if buffer was emptied while we waited
        sem_post(&s->mutex);
        return -1;
    }
    if (hostname == NULL)
        *hostname = (char*)malloc(MAX_NAME_LENGTH*sizeof(char)); // malloc space for hostname
    strcpy(*hostname, s->array[--s->top]);	  	                // CRIT SECTION accessing shared array
    sem_post(&s->mutex);      				                    // free inner mutex
    sem_post(&s->space_avail);				                    // signal space is available
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

// return true (1) if array is empty
int array_empty(array *s) {
    if (s->top < 1) return 1;
    else return 0;
}