#include "pfile.h"
#include "string.h"

void FileInit(pfile *file, char *str) {
    file->fileName = str;
    pthread_mutex_init(&file->fileLock, NULL);
    FILE* fp = fopen(file->fileName, "w");
    fclose(fp);
}

void WriteLine(pfile *file, char *str) {
    str[strcspn(str, "\n")] = 0; // remove eol char if it exists
    char* newstr = strcat(str, "\n");
    pthread_mutex_lock(&file->fileLock);
    FILE* fp = fopen(file->fileName, "a");
    fwrite(newstr, 1, strlen(newstr), fp);
    fclose(fp);
    pthread_mutex_unlock(&file->fileLock);
}