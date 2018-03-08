#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#define _XOPEN_SOURCE 500

pid_t parent_pid;
pid_t child_pid;
int task_type;
int sent = 0;
int recieved = 0;
int sent_back = 0;

void parseArguments(int argc, char **argv){
  if(argc != 3){
		printf("Wrong number of arguments - the arguments should be: L - amount of signals sent, T -task type ");
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
// we always return real time or sigusr signals, so one branching is enough
int branchTypes(int task_type, int command){
  if(task_type==3){
    if(command == 1){
      return SIGRTMIN + 5;
    }
    else{
      return SIGRTMIN + 6;
    }
  }
  else{
    if(command == 2){
      return SIGUSR1;
    }
    else{
      return SIGUSR2;
    }
  }
}
// we always use sigque or sigkill, so one branching is enough
void branchAction(int task_type, pid_t pid, int command){
  union sigval helper;
  helper.sival_int = 0;
  if(task_type == 2){
    sigqueue(pid, branchTypes(task_type, command), helper);
  }
  else{
    kill(pid, branchTypes(task_type, command));
  }
}

void signal_parser(int sigNumber, siginfo_t *signal_info, void *usercontext){
  if(sigNumber == branchTypes(task_type, 1)){
    if(signal_info->si_pid == parent_pid){
      printf("Child recieved signal 1\n");
      recieved++;
      branchAction(task_type, parent_pid, 1);
    }
    else{
      printf("Parent recieved signal 1\n");
      sent_back++;
    }
  }
  else{
    if(sigNumber == branchTypes(task_type, 2)){
      printf("Child recieved signal 2\n");
      exit(recieved);
    }
    else{
      if(sigNumber == SIGINT){
        branchAction(task_type, child_pid, 2); /// send termination signal like after ending a loop, then we end up in the above if option
      }
    }
  }
}
int main(int argc, char **argv) {

    // Arguments
    parseArguments(argc, argv);
    int L = intConversion(argv[1]);
    task_type = intConversion(argv[2]);
    if (task_type < 1 || task_type > 3) {
        printf("The task must be a number between 1 and 3.\n");
        exit(1);
    }

    // Handler for signals
    struct sigaction signal_action;
    signal_action.sa_sigaction = signal_parser;
    signal_action.sa_flags = SA_SIGINFO;
    sigemptyset(&signal_action.sa_mask);
    /// branch out options
    sigaction(branchTypes(task_type, 1), &signal_action, NULL);
    sigaction(branchTypes(task_type, 2), &signal_action, NULL);
    sigaction(SIGINT, &signal_action, NULL);

    parent_pid = getpid();
    int pid = fork();
    if(pid > 0){//parent
      for(int i = 0; i < L; i++){
        branchAction(task_type, pid, 1);
        printf("Parent sent signal 1 to childe.\n");
        sent++;
      }
      branchAction(task_type, pid, 2);
      printf("Parent sent signal 2 to childe.\n");
      int childStatus;
      wait(&childStatus);
      recieved = WEXITSTATUS(childStatus);
    }
    else{
      //on standby to recieve signals, will be terminated by sending SIGUSR2 signal
      while(1);
    }
    printf("sent = %d, recieved = %d, sent back = %d", sent, recieved, sent_back);
    return 0;
}
