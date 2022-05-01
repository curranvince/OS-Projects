#include "multi-lookup.h"

int main(int argc, char *argv[]) {
    shared_resources *shared;
    pthread_t req_threads[MAX_REQUESTER_THREADS];    
    pthread_t res_threads[MAX_RESOLVER_THREADS];
    char *reqEnd, *resEnd;
    long l_num_requesters, l_num_resolvers;
    int num_requesters, num_resolvers;
    struct timeval begin, end;
    shared = malloc(sizeof(shared_resources));
    // start timer
    gettimeofday(&begin, 0);
    // exit if we get the wrong # of cmd line arguments
    if (argc < 6 || argc > (MAX_INPUT_FILES + 5)) {
        fprintf(stderr, "ERROR: Got bad number of arguments\n");
        fprintf(stderr, "Usage: ./multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]\n");
        exit(-1);
    }
    // convert # req/res input strings to longs
    l_num_requesters = strtol(argv[1], &reqEnd, 10);
    l_num_resolvers = strtol(argv[2], &resEnd, 10);
    // ensure conversion worked
    if (reqEnd == argv[1] || resEnd == argv[2]) {
        fprintf(stderr, "ERROR: Got string when expecting int\n");
        fprintf(stderr, "Usage: ./multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]\n");
        exit(-1);
    }
    // convert # req/res to int & ensure within bounds
    num_requesters = (int) l_num_requesters;
    shared->num_requestersDelta = (int) l_num_requesters;
    num_resolvers = (int) l_num_resolvers;
    if (num_resolvers < 1 || num_requesters < 1 || num_requesters > MAX_REQUESTER_THREADS || num_resolvers > MAX_RESOLVER_THREADS) {
        fprintf(stderr, "ERROR: Number of threads determined to be out of bounds (please use 1-10)\n");
        fprintf(stderr, "Usage: ./multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]\n");
        exit(-1);
    }
    // create log files
    FileInit(&shared->req_log, argv[3]);
    FileInit(&shared->res_log, argv[4]);
    // init filename array
    if (stack_init(&shared->filenames) < 0) exit(1);
    // init hostname array
    if (array_init(&shared->hostnames) < 0) exit(1);
    // create stack of filenames
    for (int i = 0; i < (argc-5); i++) {
        stack_put(&shared->filenames, argv[i+5]);
    }
    pthread_mutex_init(&shared->counterLock, NULL);
    // create requesters
    for (int i = 0; i < num_requesters; i++) {
        pthread_create(&req_threads[i], NULL, &request, shared);
    }
    // create resolvers
    for (int i = 0; i < num_resolvers; i++) {
        pthread_create(&res_threads[i], NULL, &resolve, shared);
    }
    // join requesters
    for (int i = 0; i < num_requesters; i++) {
        pthread_join(req_threads[i], NULL);
    }
    stack_free(&shared->filenames);
    // join resolvers
    for (int i = 0; i < num_resolvers; i++) {
        pthread_join(res_threads[i], NULL);
    }  
    // free remaining memory
    array_free(&shared->hostnames);
    free(shared);
    // calculate time to run and exit w/ success
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long micros = end.tv_usec - begin.tv_usec;
    double timeTaken = seconds + micros*1e-6;
    printf("./multi-lookup: total time is %.6f seconds\n", timeTaken);
    exit(0);
}