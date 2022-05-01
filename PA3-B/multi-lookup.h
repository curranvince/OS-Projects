#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

/*
The file names specified by <data file> are passed to the pool of requester threads which place information into a shared data area. 
Resolver threads read the shared data area and find the corresponding IP address.
*/

#include "array.h"
#include "pstack.h"
#include "pfile.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

typedef struct {
    pfile req_log;
    pfile res_log;
    pstack filenames;
    array hostnames;
    pthread_mutex_t counterLock;
    int num_requestersDelta;
} shared_resources;

/* Requesters take a file from input
   And write hostnames to buffer & requester log
   Then checks for more files to service  
*/
void * request(void * arg) {
    int files_serviced = 0; // track how many files this thread services
    FILE *service_file;
    char *file_to_service = (char*)malloc(MAX_NAME_LENGTH*sizeof(char));
    char *line = NULL; // getline will malloc this for us
    size_t len = 0;
    shared_resources *shared = arg;
    // check if theres a file to service
    while (stack_get(&shared->filenames, &file_to_service) == 0) {
        // read hostnames from file
        service_file = fopen(file_to_service, "r");
        while (getline(&line, &len, service_file) != -1) {
            // write hostnames to buffer & req log
            line[strcspn(line, "\n")] = 0; // remove eol char if it exists
            array_put(&shared->hostnames, line);
            WriteLine(&shared->req_log, line);
        }
        fclose(service_file);
        files_serviced++;
    }

    free(file_to_service);
    free(line);

    printf("thread %ld serviced %d files\n", pthread_self(), files_serviced);
    pthread_mutex_lock(&shared->counterLock); // put lock around iterating counter
    shared->num_requestersDelta--;
    pthread_mutex_unlock(&shared->counterLock);
    pthread_exit(NULL);
}

/* Resolvers take a hostname from buffer, resolve it
   then write hostnames/resolved IPS to resolver log
*/
void * resolve(void * arg) {
    int resolved_names = 0;
    char *hostname = (char*)malloc(MAX_NAME_LENGTH*sizeof(char));
    char *resolvedName = (char*)malloc(MAX_NAME_LENGTH*sizeof(char));
    shared_resources *shared = arg;
    while (1) {
        if (shared->num_requestersDelta == 0 && array_empty(&shared->hostnames) == 1) {
            break;
        };
        if (array_get(&shared->hostnames, &hostname) == 0) {
            // if lookup fails, write that instead of IP
            if (dnslookup(hostname, resolvedName, MAX_IP_LENGTH) == -1) {
                strcpy(resolvedName, "NOT_RESOLVED");
            }
            char *str = strcat(strcat(hostname, ", "), resolvedName);
            WriteLine(&shared->res_log, str);
            resolved_names++;
        }
    }

    free(hostname);
    free(resolvedName);

    printf("thread %ld resolved %d hostnames\n", pthread_self(), resolved_names);
    pthread_exit(NULL);
}

#endif