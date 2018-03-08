#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "systemver.h"

void systemShuffle(int rAmount, int fileDescriptor, size_t rSize){

  srand(time(NULL));
  ssize_t rwHelper; /// syscall read outputs this to this type, useful for checking errors
  /// 2 records at a time
  char *record1 = calloc(rSize, sizeof(char));
  char *record2 = calloc(rSize, sizeof(char));

  /// doing the loop 5 times to increase time and randomness
  int iterator = 0;
  while(iterator < 5){
    for(int i = 0; i < rAmount; i++){

      /// reading record1
      if(lseek(fileDescriptor, i*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = read(fileDescriptor, record1, rSize);
      if(rwHelper == -1){
        perror("Reading from file failed : ");
        exit(1);
      }

      ///randomizing record for swap
      int randomRecord = rand()%rAmount;

      /// reading record2
      if(lseek(fileDescriptor, randomRecord*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = read(fileDescriptor, record2, rSize);
      if(rwHelper == -1){
        perror("Reading from file failed : ");
        exit(1);
      }

      /// swap
      if(lseek(fileDescriptor, randomRecord*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = write(fileDescriptor, record1, rSize);
      if(rwHelper == -1){
        perror("Writing to file failed : ");
        exit(1);
      }
      /// swapping pointer to record1 location
      if(lseek(fileDescriptor, i*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = write(fileDescriptor, record2, rSize);
      if(rwHelper == -1){
        perror("Writing to file failed : ");
        exit(1);
      }
    }
    iterator += 1;
  }
}

void systemSort(int rAmount, int fileDescriptor, size_t rSize){

  ssize_t rwHelper; /// syscall read outputs this to this type, useful for checking errors
  /// 2 records at a time
  char *record1 = calloc(rSize, sizeof(char));
  char *record2 = calloc(rSize, sizeof(char));

  /// easiest possible bubblesort
  for(int i = rAmount - 1; i >= 0; i--){
    /// putting the pointer at the start of the file
    if(lseek(fileDescriptor, 0, SEEK_SET)== -1){
      perror("Operation on file pointer failed : ");
      exit(1);
    }

    for(int j = 0; j < i; j++){
      /// reading record1
      if(lseek(fileDescriptor, j*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = read(fileDescriptor, record1, rSize);
      if(rwHelper == -1){
        perror("Reading from file failed : ");
        exit(1);
      }

      /// reading record2
      if(lseek(fileDescriptor, (j+1)*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = read(fileDescriptor, record2, rSize);
      if(rwHelper == -1){
        perror("Reading from file failed : ");
        exit(1);
      }
      /// checking if swap is required
      if(record1[0]>record2[0]){
        if(lseek(fileDescriptor, j*rSize, SEEK_SET)== -1){
          perror("Operation on file pointer failed : ");
          exit(1);
        }
        rwHelper = write(fileDescriptor, record2, rSize);
        if(rwHelper == -1){
          perror("Writing to file failed : ");
          exit(1);
        }
        rwHelper = write(fileDescriptor, record1, rSize);
        if(rwHelper == -1){
          perror("Writing to file failed : ");
          exit(1);
        }
      }
    }
  }

}
