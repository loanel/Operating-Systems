#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <memory.h>
#include <sys/wait.h>
#define _MAX_ARGUMENTS 5
#define _MAX_COMMANDS 20


int getParts(char *command_line, char **parts){
    parts[0] = strtok(command_line,"|\n");
    int i = 0;

    while(parts[i] != NULL && i < _MAX_COMMANDS){
        i++;
        parts[i] = strtok(NULL,"|");
    }
    return i;
}

char** getArguments(char *part){
    char **arguments = malloc(_MAX_ARGUMENTS*sizeof(char*));
    arguments[0] = strtok(part," \n");

    int i = 0;
    /// will only take 5 arguments
    while(arguments[i] != NULL && i < _MAX_ARGUMENTS){
        i++;
        arguments[i] = strtok(NULL," \n");
    }
    return arguments;
}

void spawn_process(int input, int output, char **part){
  /// splitting command line part into arguments
  pid_t pid;
  pid = fork();
  if(pid == 0){
    /// changing STDIN to input if we arent at the start
    if(input != 0){
      dup2(input, 0);
      close(input);
    }
    /// changing STDOUT
    if(output != 1){
      dup2(output, 1);
      close(output);
    }
    /// execute
    if(execvp(part[0], part) == -1){
      perror("Failed to exec one of the commands\n");
      exit(1);
    }
  }
  else wait(NULL);
}

void fork_pipelines(char *command_line){
  // splitting command line into parts
  char **parts = malloc(_MAX_COMMANDS*sizeof(char*));
  int parts_amount = getParts(command_line, parts);
  if(parts_amount > _MAX_COMMANDS){
    printf("Maximum number of commands exceeded, must be below 20");
  }
  /// setting up variables required for piping;
  int i;
  int input, fd[2];
  input = 0;
  for(i = 0; i < parts_amount - 1; ++i){
    pipe(fd);
    spawn_process(input, fd[1], getArguments(parts[i]));
    close(fd[1]);
    input = fd[0];
  }
  spawn_process(input, 1, getArguments(parts[i]));
}

int main(int argc, char **argv){
  while(1){
    char data[1024];
    if(fgets(data, 1024, stdin)){
      fork_pipelines(data);
      sleep(1);
    }
    else{
      printf("WHY ARENT I WAITING");
      exit(1);
    }
  }
  return 0;
}
