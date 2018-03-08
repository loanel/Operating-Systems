#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/// command implementation :

struct flock *generateLockStructure(off_t byteNumber, int fileDescriptor, short lockType){

  struct flock *newLock = malloc(sizeof(struct flock));
  newLock->l_type = lockType;
  newLock->l_len = 1;
  newLock->l_start = byteNumber;
  newLock->l_whence = SEEK_SET;
  return newLock;

}

char readChar(int byteNumber, int fileDescriptor){

  if(lseek(fileDescriptor, byteNumber, SEEK_SET)== -1){
    perror("Operation on filepointer failed : ");
    exit(1);
  }
  struct flock *newLock = generateLockStructure(byteNumber, fileDescriptor, F_RDLCK);
  if(fcntl(fileDescriptor, F_GETLK, newLock) == -1){
    printf("Failed to get data on locks on byte");
  }
  else  if(newLock->l_type == F_WRLCK){
    printf("File potentially writelocked - no permisssion to read in it");
  }
        else{
          ssize_t rwHelper;
          char *record = malloc(sizeof(char));
          rwHelper = read(fileDescriptor, record, 1);
          if(rwHelper == -1){
            perror("Reading from file failed : ");
            exit(1);
          }else{
            if(rwHelper == 0){
              perror("End of file :");
              exit(1);
            }
          }
          return record[0];
        }
  return ' ';
}

void writeChar(int byteNumber, int fileDescriptor, char character){

  if(lseek(fileDescriptor, byteNumber, SEEK_SET)== -1){
    perror("Operation on filepointer failed : ");
    exit(1);
  }
  struct flock *newLock = generateLockStructure(byteNumber, fileDescriptor, F_WRLCK);
  if(fcntl(fileDescriptor, F_GETLK, newLock) == -1){
    printf("Failed to get data on locks on byte");
  }
  else  if(newLock->l_type != F_UNLCK){
            printf("File not unlocked - no permisssion to write in it");
        }
        else{
          if(write(fileDescriptor, &character, 1) == -1){
            printf("Couldn't write to file");
          }
          else{
            printf("Character written sucesfully");
          }
        }
}

void outputLocks(int fileDescriptor){

  int fileSize = lseek(fileDescriptor, 0, SEEK_END); /// \o/SO\o/
  struct flock *fileLock = NULL;

  for(int i = 0; i < fileSize; i++){
    printf("A");
    fileLock = generateLockStructure(i, fileDescriptor, F_WRLCK);
    if(fcntl(fileDescriptor, F_GETLK, fileLock) == -1){
      perror("Reading filelocks failed");
      break;
    }
    if(fileLock->l_type == F_RDLCK){
      printf("B");
      printf("Read lock : byte - %d, ProcessID - %d\n", i, fileLock->l_pid);
    }
    else if(fileLock->l_type == F_WRLCK){
            printf("Write lock : byte - %d, ProcessID - %d\n", i, fileLock->l_pid);
          }
  }
  free(fileLock);
}

void printHelp(){
  printf("Choose operation :\n1 - set readlock (shared) on byte\n2 - set writelock on byte\n3 - output locked bytes\n4 - free lock on chosen byte\n5 - read byte from file\n6 - change byte in file\n7 - help\n8 - quit\n");
}

int inputInteger(){

  char input[64];
  int integer;
  if(fgets(input, sizeof(input), stdin) == NULL){
    printf("Error during reading argument from stdin");
    exit(1);
  }
  int readHelper = sscanf(input, "%d", &integer); /// \o/SO\o/
  if(readHelper <= 0){
    printf("Failure while parsing integer input");
    exit(1);
  }

  return integer;
}

char inputChar(){
  char input[64];
  char character;
  if(fgets(input, sizeof(input), stdin) == NULL){
    printf("Error during reading argument from stdin");
    exit(1);
  }
  int readHelper = sscanf(input, "%c", &character); /// \o/SO\o/
  if(readHelper <= 0){
    printf("Failure while parsing integer input");
    exit(1);
  }

  return character;
}

int main(int argc, char** argv){

  char input[64];
  char command;
  struct flock *fileLock;
  int byteNumber;
  if(argc != 2){
    printf("Wrong number of arguments inputed");
    exit(1);
  }
  printf("File locking interface, executed on %s", argv[1]);

  int fileDescriptor = open(argv[1], O_RDWR);
  if(fileDescriptor == -1){
    perror("Failed to open file :");
    exit(1);
  }

  printHelp();
  while(1){
    /// fgets + sscanf better that scanf \o/SO\o/
    if(fgets(input, sizeof(input), stdin) == NULL){
      printf("Error during reading argument from stdin");
      exit(1);
    }
    command = input[0];
    switch(command){
    case '1': /// READLOCK

      printf("Choose byte\n");
      byteNumber = inputInteger();
      if(byteNumber<0){
        printf("Bytenumber has to be nonnegative");
        break;
      }

      fileLock = generateLockStructure(byteNumber, fileDescriptor, F_RDLCK);

      printf("Choose course of action :\n 1 - try to lock, leave if impossible \n 2 - wait until lock possible\n");
      if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error during reading argument from stdin");
        exit(1);
      }
      command = input[0];
      if(command == '1'){
        if(fcntl(fileDescriptor, F_SETLK, fileLock) == -1){
          printf("Setting reading lock failed");
          break;
        }
      }
      else {
           if(command == '2'){
              if(fcntl(fileDescriptor, F_SETLKW, fileLock) == -1){
                printf("Setting reading lock failed");
                break;
              }
           }
           else{
             printf("Wrong argument - has to be 1 or 2");
           }
      }
      fsync(fileDescriptor);
      printf("Read lock set on chosen byte\n");
      break;

    case '2': /// WRITELOCK

      printf("Choose byte\n");
      byteNumber = inputInteger();
      if(byteNumber<0){
        printf("Bytenumber has to be nonnegative");
        break;
      }

      fileLock = generateLockStructure(byteNumber, fileDescriptor, F_WRLCK);

      printf("Choose course of action :\n 1 - try to lock, leave if impossible \n 2 - wait until lock possible\n");
      if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error during reading argument from stdin");
        exit(1);
      }
      command = input[0];
      if(command == '1'){
        if(fcntl(fileDescriptor, F_SETLK, fileLock) == -1){
          printf("Setting writing lock failed");
          break;
        }
      }
      else {
           if(command == '2'){
              if(fcntl(fileDescriptor, F_SETLKW, fileLock) == -1){
                printf("Setting writing lock failed");
                break;
              }
           }
           else{
              printf("Wrong argument - has to be 1 or 2");
           }
      }
      fsync(fileDescriptor);
      printf("Write lock set on chosen byte\n");
      break;

    case '3': /// WRITE EVERY LOCK, requires second terminal

      printf("Filelocks currently set :\n");
      outputLocks(fileDescriptor);
      break;

    case '4': /// unlock lock

      printf("Choose byte\n");
      byteNumber = inputInteger();
      if(byteNumber<0){
        printf("Bytenumber has to be nonnegative");
        break;
      }

      fileLock = generateLockStructure(byteNumber, fileDescriptor, F_UNLCK);
      if(fcntl(fileDescriptor, F_SETLK, fileLock) == -1){
        printf("Unlocking failed");
        break;
      }
      fsync(fileDescriptor);
      printf("Byte %d unlocked", byteNumber);
      break;

    case '5': /// read char

      printf("Choose byte\n");
      byteNumber = inputInteger();
      if(byteNumber<0){
        printf("Bytenumber has to be nonnegative");
        break;
      }
      char helper = readChar(byteNumber, fileDescriptor);
      if(helper != ' '){
        printf("Character on %d is %c\n", byteNumber, helper);
      }
      break;

    case '6': /// write char

      printf("Choose byte\n");
      byteNumber = inputInteger();
      if(byteNumber<0){
        printf("Bytenumber has to be nonnegative");
        break;
      }
      printf("What character do you want to write?");
      writeChar(byteNumber, fileDescriptor, inputChar());
      fsync(fileDescriptor);
      break;

    case '7':
      printHelp();
      break;

    case '8':
      printf("Finishing");
      if(close(fileDescriptor) == -1){
        perror("Error while closing file: ");
        exit(1);
      }
      exit(1);
      break;
    default :
      printf("Choose another option from 1 to 8 - 7 for help");
      break;
    }
    printf("\n");
  }
}
