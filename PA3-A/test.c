#include "array.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

array my_stack;
char* str;
int times_to_run = 10;  
int max_threads = 20;
int num_strings = 500;
int per_thread_p;
int per_thread_c;

// method for threads to produce "HelloWorld" to stack
void * produce(void * arg) {
    for (int i = 0; i < per_thread_p; i++) {
	array_put(&my_stack, "HelloWorld");
    }
}

// have threads try to put really long str on stack
void * producelong(void * arg) {
    char longstr[2000];
    char ch = 'a';
    for (int i = 0; i < 2000; i++) {
	strncat(longstr, &ch, 1);
    }
    for (int i = 0; i < per_thread_p; i++) {
        array_put(&my_stack, longstr);
    }
}

// method for thread to pop from stack
void * consume(void * arg) {
    for (int i = 0; i < per_thread_c; i++) {
        array_get(&my_stack, &str);
    }
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < times_to_run; i++) {
        pthread_t pthreads[max_threads];    
        pthread_t cthreads[max_threads];	
        if (array_init(&my_stack) < 0) exit(-1);                // init stack, exit if failed
        /* Test equal pros/cons */
        printf("Starting equal pro/con test\n");
        per_thread_c = num_strings/max_threads;
        per_thread_p = num_strings/max_threads;
        for (int i = 0; i < max_threads; i++) {
            pthread_create(&pthreads[i], NULL, &produce, NULL);
            pthread_create(&cthreads[i], NULL, &consume, NULL);
        }
        for (int i = 0; i < max_threads; i++) {
	        pthread_join(pthreads[i], NULL);
            pthread_join(cthreads[i], NULL);
        }
        printf("Completed equal pro/con test\n\n\n");
        /* Test long string names */
        printf("Starting test for long strings");
        for (int i = 0; i < max_threads; i++) {
            pthread_create(&pthreads[i], NULL, &producelong, NULL);
        }
        for (int i = 0; i < max_threads; i++) {
            pthread_join(pthreads[i], NULL);
        } 
        printf("Completed long string test\n\n\n");
        /* Test 1/2 consumers */
        printf("Starting 1/2 consumers test\n");
        per_thread_p = num_strings/max_threads;
        per_thread_c = num_strings/(max_threads/2);
        for (int i = 0; i < max_threads; i++) {
            pthread_create(&pthreads[i], NULL, &produce, NULL);
            if (i % 2 == 0) pthread_create(&cthreads[i], NULL, &consume, NULL);
        }
        for (int i = 0; i < max_threads; i++) {
            pthread_join(pthreads[i], NULL);
            if (i % 2 == 0) pthread_join(cthreads[i], NULL);
        }
        printf("Completed 1/2 consumers test\n");
        /* Test 1/2 producers */
        printf("Starting 1/2 producers test\n");
        per_thread_p = num_strings/(max_threads/2);
        per_thread_c = num_strings/max_threads;    
        for (int i = 0; i < max_threads; i++) {
            if (i % 2 == 0) pthread_create(&pthreads[i], NULL, &produce, NULL);
            pthread_create(&cthreads[i], NULL, &consume, NULL);
        }
        for (int i = 0; i < max_threads; i++) {
            if (i % 2 == 0) pthread_join(pthreads[i], NULL);
            pthread_join(cthreads[i], NULL);
        }
        printf("Completed 1/2 producers test\n\n\n");
        /* Test max producers, 1 consumer */
        printf("Starting max producers, 1 consumer test\n");
        per_thread_p = num_strings/max_threads;
        per_thread_c = num_strings;
        for (int i = 0; i < max_threads; i++) {
            pthread_create(&pthreads[i], NULL, &produce, NULL);
            if (i == 0) pthread_create(&cthreads[i], NULL, &consume, NULL);
        }
        for (int i = 0; i < max_threads; i++) {
            pthread_join(pthreads[i], NULL);
            if (i == 0) pthread_join(cthreads[i], NULL);
        }
        printf("Completed max producers, 1 consumer test\n\n\n");
        /* Test 1 producer, max consumers */
        printf("Starting 1 producer, max consumers test\n");
        per_thread_p = num_strings;
        per_thread_c = num_strings/max_threads;
        for (int i = 0; i < max_threads; i++) {
            if (i == 0) pthread_create(&pthreads[i], NULL, &produce, NULL);
            pthread_create(&cthreads[i], NULL, &consume, NULL);
        }
        for (int i = 0; i < max_threads; i++) {
            if (i == 0) pthread_join(pthreads[i], NULL);
            pthread_join(cthreads[i], NULL);
        }
        printf("Completed 1 producer, max consumers test\n");
        array_free(&my_stack);                                  // destroy stack
     }
     exit(0);
}
