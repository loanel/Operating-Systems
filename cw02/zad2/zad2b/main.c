#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <ftw.h>

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

static int fileSize;

int parseDirectory(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){

    if(sb == NULL){
      return 0;
    }

    if(S_ISREG(sb->st_mode)){
      if((int)sb->st_size < fileSize){
        char *permissionBits = malloc(sizeof(char)*10);

        permissionBits[0] = sb->st_mode & S_IRUSR ? 'R' : '-'; /// mask 100000000 , ongoing for other etc
        permissionBits[1] = sb->st_mode & S_IWUSR ? 'W' : '-';
        permissionBits[2] = sb->st_mode & S_IXUSR ? 'X' : '-';

        permissionBits[3] = sb->st_mode & S_IRUSR ? 'R' : '-';
        permissionBits[4] = sb->st_mode & S_IWUSR ? 'W' : '-';
        permissionBits[5] = sb->st_mode & S_IXUSR ? 'X' : '-';

        permissionBits[6] = sb->st_mode & S_IRUSR ? 'R' : '-';
        permissionBits[7] = sb->st_mode & S_IWUSR ? 'W' : '-';
        permissionBits[8] = sb->st_mode & S_IXUSR ? 'X' : '-';
        permissionBits[9] = '\0'; /// required at end of string
        printf("Filepath : %s, Filesize : %d, Permissions : %s, Last time modified : %s\n",
                fpath,
                (int)sb->st_size,
                permissionBits,
                ctime((&sb->st_mtime)));
      }
    }
    return 0;
}


void runCommands(char **argv){

  fileSize = intConversion(argv[2]);
  nftw(argv[1], parseDirectory, 16, FTW_PHYS);

}

int main(int argc, char** argv){

  parseArguments(argc, argv);
  runCommands(argv);

}
