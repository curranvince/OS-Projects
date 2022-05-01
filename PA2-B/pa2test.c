#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX 1024

int main(int argc, char *argv[]) {
  int count = 0;
  int errnum, ret_code, iinput, file, offset, whence;
  char cinput, ch;
  char *buffer = NULL;
  char writebuffer[MAX];
  if (argc != 2) {
    fprintf(stderr, "Error: Program called with unexpected input\n");
    return 1;
  } else {
    file = open(argv[1], O_RDWR);
    if (file < 0) {
      errnum = errno;
      fprintf(stderr, "Error opening file: %s\n", strerror(errnum));
      return 1;
    } else {
      do {
        // prompt user for read write seek option, check for ctr-d exit
        printf("Option (r for read, w for write, s for seek): ");
        ret_code = scanf("%c", &cinput);
	if (ret_code == EOF) {
	  printf("\n");
	  close(file);
	  return 0;
	}
	getchar();
        if (cinput == 'r') {
	  // get user input for num bytes to read, check for exit
	  printf("Enter the number of bytes you want to read: ");
	  ret_code = scanf("%d", &iinput);
          if (ret_code == EOF) {
	    printf("\n");
            close(file);
            return 0;
          }
  	  getchar();
	  // allocate buffer to read to
	  buffer = malloc(iinput+1);
	  if (buffer == NULL) {
	    fprintf(stderr, "Error allocating memory\n");	  
	  } else {
	    // read file into buffer
	    ssize_t new_size = read(file, buffer, iinput);
	    // print buffer to console and reset
            printf("%s\n", buffer);
	    buffer[new_size++] = '\0'; 
	  }
	  free(buffer);
	} else if (cinput == 'w') {
	  // get user input for string to write, check for exit
	  printf("Enter the string you want to write: ");
	  ret_code = scanf("%[^\n]%*c", writebuffer);
          if (ret_code == EOF) {
            printf("\n");
	    close(file);
            return 0;
          }
	  // write string to file
	  write(file, writebuffer, strlen(writebuffer));
	} else if (cinput == 's') {
	  // if user wants to seek
	  // prompt for offset value, allowing exit
	  printf("Enter an offset value: ");
	  ret_code = scanf("%d", &offset);
          if (ret_code == EOF) {
	    printf("\n");
	    close(file);
            return 0;
          }
          getchar();
	  do {
	    // prompt for whence value
	    printf("Enter a whence value: ");
            ret_code = scanf("%d", &whence);
            if (ret_code == EOF) {
	      printf("\n");
	      close(file);
              return 0;
            }
            getchar();
	  } while (whence != 0 && whence != 1 && whence != 2);
	  // perform file seek
	  lseek(file, offset, whence);
	} else {
	  printf("Please enter valid input\n");
        }
      } while (1);
    }
  }
  return 0;
}
