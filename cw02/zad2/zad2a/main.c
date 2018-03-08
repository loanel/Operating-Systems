#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>


int numeral(char *pNumber){
  for(int i = 0; i < strlen(pNumber); i++){
    if(pNumber[i]<'0'||pNumber[i]>'9') return 0;
  }
  return 1;
}

void parseArguments(int argcount, char **arguments){

  if(argcount!=3){
    perror("The required number of arguments is 2 - path and filesize");
    exit(1);
  }
  DIR *currentDirectory = opendir(arguments[1]);
  if(currentDirectory == NULL){
    perror("Failed to open file :");
    exit(1);
  }
  if(closedir(currentDirectory) == -1){
    perror("Current directory couldnt be closed after testing if it exists : ");
    exit(1);
  }
  if(numeral(arguments[2])!=1){
    perror("The number of records has to be a positive number");
    exit(1);
  }
}

int intConversion(char *number){
  int convertedNumber = 0;
  for(int i = 0; i < strlen(number); i++){
    convertedNumber = convertedNumber * 10 + (number[i] - 48);
  }
  return convertedNumber;
}

void parseDirectory(char *filePath, int fileSize){

  DIR *currentDirectory = opendir(filePath);
  struct stat statistics;
  char *currentFileName;
  char concat[4096];
  if(currentDirectory == NULL){
    perror("Failed to open file :");
    exit(1);
  }
  while(1){
    struct dirent *file = readdir(currentDirectory);
    if(file == NULL) break;
    currentFileName = file->d_name;///char array so ->
    /// write from buffer into a string for at most 4096 bytes
    snprintf(concat, 4096, "%s/%s", filePath, currentFileName);
    if(lstat(concat, &statistics) == -1){
      perror("Problem accesing information about file : ");
      exit(1);
    }

    if(S_ISREG(statistics.st_mode)){
      if((int)statistics.st_size < fileSize){
        char *permissionBits = malloc(sizeof(char)*10);

        permissionBits[0] = statistics.st_mode & S_IRUSR ? 'R' : '-'; /// mask 100000000 , ongoing for other etc
        permissionBits[1] = statistics.st_mode & S_IWUSR ? 'W' : '-';
        permissionBits[2] = statistics.st_mode & S_IXUSR ? 'X' : '-';

        permissionBits[3] = statistics.st_mode & S_IRUSR ? 'R' : '-';
        permissionBits[4] = statistics.st_mode & S_IWUSR ? 'W' : '-';
        permissionBits[5] = statistics.st_mode & S_IXUSR ? 'X' : '-';

        permissionBits[6] = statistics.st_mode & S_IRUSR ? 'R' : '-';
        permissionBits[7] = statistics.st_mode & S_IWUSR ? 'W' : '-';
        permissionBits[8] = statistics.st_mode & S_IXUSR ? 'X' : '-';
        permissionBits[9] = '\0';  /// required at end of string
        printf("Filepath : %s, Filesize : %d, Permissions : %s, Last time modified : %s\n",
                concat,
                (int)statistics.st_size,
                permissionBits,
                ctime((&statistics.st_mtime)));
      }
    }
    else{
      if(S_ISDIR(statistics.st_mode)){
        if(strcmp(currentFileName, "..") != 0 && strcmp(currentFileName, ".") != 0){
          char next[4096];
          int nullsign = snprintf(next, 4096, "%s/%s", filePath, currentFileName);
          next[nullsign] = '\0';
          parseDirectory(next, fileSize);
        }
      }
    }
  }
  if(closedir(currentDirectory) == -1){
    perror("Current directory couldnt be closed : ");
    exit(1);
  }
}

void runCommands(char **argv){

  int fileSize = intConversion(argv[2]);
  parseDirectory(argv[1], fileSize);
}

int main(int argc, char** argv){
  parseArguments(argc, argv);
  runCommands(argv);
}
