#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "libraryver.h"

void libraryShuffle(int rAmount, FILE *file, size_t rSize){

  srand(time(NULL));
  size_t rwHelper; /// syscall read outputs this to this type, useful for checking errors
  /// 2 records at a time
  char *record1 = calloc(rSize, sizeof(char));
  char *record2 = calloc(rSize, sizeof(char));
  /// doing the loop 5 times to increase time and randomness
  int iterator = 0;
  while(iterator < 5){
    for(int i = 0; i < rAmount; i++){

      /// reading record1
      if(fseek(file, i*rSize, SEEK_SET)== -1){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = fread(record1, rSize, 1, file);
      if(rwHelper == -1){
        perror("Reading from file failed : ");
        exit(1);
      }

      ///randomizing record for swap
      int randomRecord = rand()%rAmount;

      /// reading record2
      if(fseek(file, randomRecord*rSize, SEEK_SET)!=0){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = fread(record2, rSize, 1, file);
      if(rwHelper == 0){
        perror("Reading from file failed : ");
        exit(1);
      }

      /// swap
      if(fseek(file, randomRecord*rSize, SEEK_SET)!=0){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = fwrite(record1, rSize, 1, file);
      if(rwHelper == 0){
        perror("Writing to file failed : ");
        exit(1);
      }
      /// swapping pointer to record1 location
      if(fseek(file, i*rSize, SEEK_SET)!=0){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = fwrite(record2, rSize, 1, file);
      if(rwHelper == 0){
        perror("Writing to file failed : ");
        exit(1);
      }
    }
    iterator += 1;
  }
}

void librarySort(int rAmount, FILE *file, size_t rSize){

  size_t rwHelper; /// syscall read outputs this to this type, useful for checking errors
  /// 2 records at a time
  char *record1 = calloc(rSize, sizeof(char));
  char *record2 = calloc(rSize, sizeof(char));

  /// easiest possible bubblesort
  for(int i = rAmount - 1; i >= 0; i--){

    /// putting the pointer at the start of the file
    if(fseek(file, 0, SEEK_SET) != 0){
      perror("Operation on file pointer failed : ");
      exit(1);
    }

    for(int j = 0; j < i; j++){
      /// reading record1
      if(fseek(file, j*rSize, SEEK_SET) != 0){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = fread(record1, rSize, 1, file);
      if(rwHelper == 0){
        perror("Reading from file failed : ");
        exit(1);
      }

      /// reading record2
      if(fseek(file, (j+1)*rSize, SEEK_SET) != 0){
        perror("Operation on file pointer failed : ");
        exit(1);
      }
      rwHelper = fread(record2, rSize, 1, file);
      if(rwHelper == 0){
        perror("Reading from file failed : ");
        exit(1);
      }
      /// checking if swap is required
      if(record1[0]>record2[0]){
        if(fseek(file, j*rSize, SEEK_SET) != 0){
          perror("Operation on file pointer failed : ");
          exit(1);
        }
        rwHelper = fwrite(record2, rSize, 1, file);
        if(rwHelper == 0){
          perror("Writing to file failed : ");
          exit(1);
        }
        rwHelper = fwrite(record2, rSize, 1, file);
        if(rwHelper == 0){
          perror("Writing to file failed : ");
          exit(1);
        }
      }
    }
  }

}
