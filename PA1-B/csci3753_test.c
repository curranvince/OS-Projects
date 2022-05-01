#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc == 3 && atoi(argv[1]) && atoi(argv[2])) {
	int* result;
        syscall(334, atoi(argv[1]), atoi(argv[2]), result);
    } else {
	printf("Please pass two integers to the command line next time.\n");
    }
}
