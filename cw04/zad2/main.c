#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#define _XOPEN_SOURCE 500

/// global variables
/// required 2 pid holder arrays, pid_holder is required to know where to send signals,
/// req_holder is required because we need to know which processes are requesting before K
int *pid_holder;
int *req_holder;
int parent_pid; /// id of parent
int requests_reached = 0; /// reply immedietly after reaching M signals
int req_amount = 0; /// requests counter

int N;

int intConversion(char *number){
  int convertedNumber = 0;
  for(int i = 0; i < strlen(number); i++){
    convertedNumber = convertedNumber * 10 + (number[i] - 48);
  }
  return convertedNumber;
}

void parseArguments(int argc, char **argv){
  if(argc != 3){
		printf("Wrong number of arguments - the arguments should be: N - amount of children, M - amount of requests");
		exit(1);
	}
}

void signal_parser(int sigNumber, siginfo_t *signal_info, void *usercontext){ ///usercontext commonly never used
    if(sigNumber == SIGUSR1){ /// signal sent by childe to parent, the 'request'
        if(requests_reached == 0){
          printf("Adding request to table from %d\n", signal_info->si_pid);
          req_holder[signal_info->si_value.sival_int] = 1;
          req_amount++;
        }
        else{
          printf("Instantly continue the child\n");
          while(kill(signal_info->si_pid, SIGUSR2)!=0);
        }
    }
    else{
      if(sigNumber == SIGUSR2){ /// signal sent by childe to parent, recieved random SIGRT from childe
        printf("Child %d recieved permission from parent\n", getpid());
      }
      else{
        if(SIGRTMIN <= sigNumber && sigNumber <= SIGRTMAX){
          printf("Recieved signal SIGRT%d from process %d\n", signal_info->si_signo-SIGRTMIN, signal_info->si_pid);
        }
      }
    }
    if(sigNumber == SIGINT){
      for(int i = 0; i < N; i++){
        kill(pid_holder[i], SIGKILL);
      }
      free(pid_holder);
      free(req_holder);
      exit(0);
    }
}
int main(int argc, char **argv) {

  parseArguments(argc, argv);
  N = intConversion(argv[1]);
  int M = intConversion(argv[2]);
  if(N < 0 || M < 0 || M > N){
    printf("Wrong arguments, N, M have to be positive integers and N has to be bigger than M");
    exit(1);
  }
  pid_holder = malloc(sizeof(int)*N); /// array for child pid's
  req_holder = malloc(sizeof(int)*N); /// array for requests from children that have to be answered after K amount
  for(int i = 0; i < N; i++){
    req_holder[i] = 0;
  }
  /// creating signal handlers
  struct sigaction signal_action;
  signal_action.sa_flags = SA_SIGINFO;  ///  use sigaction signature for easy signal masking
  signal_action.sa_sigaction = signal_parser;
  /// fill mask with empty set of signals
  sigemptyset(&signal_action.sa_mask);

  /// looking out for signals
  sigaction(SIGUSR1, &signal_action, NULL);
  sigaction(SIGUSR2, &signal_action, NULL);
  /// creating signal 'lookouts' for all user signals
  for(int i = SIGRTMIN; i <= SIGRTMAX; i++){
    sigaction(i, &signal_action, NULL);
  }

  parent_pid = getpid();
  /// do N requests
  for(int i = 0; i < N; i++){
    //sleep(1); /// less signals dissapea
    pid_t cur_pid = fork();
    if(cur_pid > 0){ /// parent process
      pid_holder[i] = cur_pid;
    }
    else{ /// childe process
      /// get pid in system
      cur_pid = getpid();
      srand((unsigned int)cur_pid);/// seed randomizer with pid
      printf("Process %d is currently working ...\n", cur_pid);
      /// work simulation
      sleep(rand()%10+1);
      /// send request to pass
      union sigval iterator_holder;
      iterator_holder.sival_int = i;
      printf("Sending request from %d\n", getpid());
  //    fflush(stdout);
      sigqueue(parent_pid, SIGUSR1, iterator_holder);

      time_t start_time;
      time(&start_time);

      int guard = sleep(10);
      if(guard == 0){
        printf("Failed to recieve signal, exit\n");
        exit(1);
      }

      time_t finish_time;
      time(&finish_time);
      /// send random signal to parent;
      kill(parent_pid, SIGRTMIN + rand()%(SIGRTMAX-SIGRTMIN));
      /// exit with time difference as code
      printf("Process %d ended with exit code %d\n", getpid(), (int)difftime(finish_time, start_time));
      exit((int)difftime(finish_time, start_time));
    }
  }
  while(req_amount < M);

  requests_reached = 1;
  int childStatus;
  for(int i = 0; i < N; i++){
    if(req_holder[i] == 1){
      kill(pid_holder[i], SIGUSR2);
      waitpid(pid_holder[i], &childStatus, 0); /// ending the process here, gives more outputs
    }
  }

  for(int i = 0; i < N; i++){
    while(waitpid(-1, NULL, 0)>0);
  }
  free(pid_holder);
  free(req_holder);

  return 0;
}
