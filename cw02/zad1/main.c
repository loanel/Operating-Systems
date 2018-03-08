#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <sys/resource.h>

#include "systemver.h"
#include "libraryver.h"

/// time measurements
double *generateTime(){

  struct rusage processUsage;
  struct timeval helper;
  getrusage(RUSAGE_SELF, &processUsage);
  double *timeOfExecution = malloc(sizeof(double)*2);

  helper = processUsage.ru_utime; // user time data to timeval helper variable (cleaner code)
  timeOfExecution[0] = (double)helper.tv_sec * 1000 + helper.tv_usec/1000;    /// we want to represent a second as a milisecond, which is why we multiply seconds by 1000
  helper = processUsage.ru_stime; // system time data to timeval helper variable
  timeOfExecution[1] = (double)helper.tv_sec * 1000 + helper.tv_usec/1000;
  return timeOfExecution;

}
void filePrintTimes(FILE *file, double *startTime, double *endTime){
  fprintf(file, "User time : %fms , System time : %fms \n", endTime[0]-startTime[0], endTime[1] - startTime[1]);
}

int numeral(char *pNumber){
  for(int i = 0; i < strlen(pNumber); i++){
    if(pNumber[i]<'0'||pNumber[i]>'9') return 0;
  }
  return 1;
}

void parseArguments(int argcount, char **arguments){

  if(argcount!=6){
    perror("The required number of arguments is 5");
    exit(1);
  }
  if(strcmp(arguments[1], "sys")!=0 && strcmp(arguments[1], "lib")!=0){
    perror("The first argument has to be sys or lib");
    exit(1);
  }
  if(strcmp(arguments[2], "generate")!=0 && strcmp(arguments[2], "shuffle")!=0 && strcmp(arguments[2], "sort")!=0){
    perror("The second argument has to be generate, shuffle or sort");
    exit(1);
  }
  if(numeral(arguments[4])!=1){
    perror("The number of records has to be a positive number");
    exit(1);
  }
  if(numeral(arguments[5])!=1){
    perror("The number of bytes in record has to be a positive number");
    exit(1);
  }
}

void generateFile(size_t rSize, int rAmount, int file){
  /// open file with random records
  int randomPool = open("/dev/urandom", O_RDONLY);
  if(randomPool == -1){
    perror("Opening /dev/urandom failed :");
    exit(1);
  }

  char *helper = calloc(rSize, sizeof(char));
  ssize_t rwHelper;
  /// output from urandom to newfile
  for(int i = 0; i < rAmount; i++){
    rwHelper = read(randomPool, helper, rSize);
    if(rwHelper == -1){
      perror("Reading from file failed : ");
      exit(1);
    }
    rwHelper = write(file, helper, rSize);
    if(rwHelper == -1){
      perror("Writing to file failed : ");
      exit(1);
    }
  }
}

int intConversion(char *number){
  int convertedNumber = 0;
  for(int i = 0; i < strlen(number); i++){
    convertedNumber = convertedNumber * 10 + (number[i] - 48);
  }
  return convertedNumber;
}

void runCommands(char **arguments){

  int rAmount = intConversion(arguments[4]);
  size_t rSize = intConversion(arguments[5]);
  FILE *resultHolder;
  double *startTimer, *endTimer;
  /// checking what has to be executed based on arguments :
  if(strcmp(arguments[2], "generate")==0){
    int fileToGenerate = open(arguments[3], O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
    generateFile(rSize, rAmount, fileToGenerate);
    printf("Generated data using syscall functions");
  } /// otherwise pick one of the 4 functions :
  else{
    if(strcmp(arguments[1],"sys")==0){

      if(strcmp(arguments[2],"shuffle")==0){

        int fileToShuffle = open(arguments[3], O_RDWR);
        resultHolder = fopen("wyniki.txt", "a");/// a - append to end of file

        fprintf(resultHolder, "Shuffle SYS : %d records of %li size\n", rAmount, rSize);
        startTimer = generateTime();
        systemShuffle(rAmount, fileToShuffle, rSize);
        endTimer = generateTime();
        filePrintTimes(resultHolder, startTimer, endTimer);

        fclose(resultHolder);
        printf("File shuffled with sys functions");

      }
      else{

        int fileToSort = open(arguments[3], O_RDWR);
        resultHolder = fopen("wyniki.txt", "a");

        fprintf(resultHolder, "Sort SYS : %d records of %li size\n", rAmount, rSize);
        startTimer = generateTime();
        systemSort(rAmount, fileToSort, rSize);
        endTimer = generateTime();
        filePrintTimes(resultHolder, startTimer, endTimer);

        fclose(resultHolder);
        printf("File sorted with sys functions");

      }

    }
    else{

      if(strcmp(arguments[2], "shuffle")==0){

        FILE *fileToShuffleLib = fopen(arguments[3], "r+"); /// r+ - open file for reading and updating
        resultHolder = fopen("wyniki.txt", "a");

        fprintf(resultHolder, "Shuffle LIB : %d records of %li size\n", rAmount, rSize);
        startTimer = generateTime();
        libraryShuffle(rAmount, fileToShuffleLib, rSize);
        endTimer = generateTime();
        filePrintTimes(resultHolder, startTimer, endTimer);

        fclose(resultHolder);
        printf("File shuffled with lib functions");

      }
      else{

        FILE *fileToSortLib = fopen(arguments[3], "r+"); /// r+ - open file for reading and updating
        resultHolder = fopen("wyniki.txt", "a");

        fprintf(resultHolder, "Sort LIB : %d records of %li size\n", rAmount, rSize);
        startTimer = generateTime();
        librarySort(rAmount, fileToSortLib, rSize);
        endTimer = generateTime();
        filePrintTimes(resultHolder, startTimer, endTimer);

        fclose(resultHolder);
        printf("File sorted with lib functions");

      }

    }
  }
}

int main(int argc, char** argv){
  parseArguments(argc, argv);
  runCommands(argv);
}
