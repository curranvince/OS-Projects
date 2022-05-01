#ifndef PFILE_H
#define PFILE_H

#include <stdio.h>
#include <pthread.h>

typedef struct {
    char* fileName; // file pointer
    pthread_mutex_t fileLock;
} pfile;

void FileInit(pfile *file, char *str);
void WriteLine(pfile *file, char *str);

#endif