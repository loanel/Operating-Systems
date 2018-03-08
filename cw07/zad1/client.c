#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include "fifoq.h"

int N;
int S;
fifo_queue *waiting_hallway;
int sem_id = -1;
int memory_id =  -1;
int cuts_amount = 0;
pid_t *children;

void sigint_handler(int sig){
  exit(0);
}

void client_leaves(){
  get_time();
  printf("Client %d ending\n", getpid());
}

void create_semaphores(){
  key_t key = ftok(getenv("HOME"), ID);
  if(key == -1){
    printf("Barber didn't recieve key\n");
    exit(1);
  }

  memory_id = shmget(key, 0, 0); /// shared memory with all permissions
  if(memory_id == -1){
    perror("Barber didn't create shared memory properly");
    exit(1);
  }

  void *void_ptr = (fifo_queue *) shmat(memory_id, NULL, 0);
  if(void_ptr == (void*)(-1)){
    printf("Barber didn't attach the shared memory");
    exit(1);
  }

  waiting_hallway = (fifo_queue *) void_ptr;

  sem_id = semget(key, 0, 0);
  if(sem_id == -1){
    perror("Barber didn't create the semaphores\n");
    exit(1);
  }
}

int main(int argc, char **argv){
  atexit(client_leaves);
  signal(SIGINT, sigint_handler);
//  signal(SIGRTMIN, got_cut);
/// fast argument parsing
  if(argc != 3){
    printf("Wrong number of arguments - the required amount is 2. Proper format - ./client  [client amount] [amount of cuts per client]");
    return 1;
  }
  S = atoi(argv[2]);
  N = atoi(argv[1]);
  if(N <= 0 || S <= 0){
    printf("The program arguments have to be positive integers");
    return 1;
  }
  printf("N = %d, S = %d \n", N, S);
  create_semaphores();

  pid_t waitpid;
  int status = 0;
  int i;
  children = malloc(sizeof(pid_t) * N);
  for(i = 0; i < N; i++){
    children[i] = 0;
    pid_t client_pid = fork();
    if(client_pid == 0){
      get_time();
      printf("Client %d begins to act...\n", getpid());
      while(cuts_amount < S){
        client_pid = getpid(); /// required actual process id, variable is copied anyway

        struct sembuf sops;
        sops.sem_num = 1; /// fifo_queue
        sops.sem_op = -1;
        sops.sem_flg = 0;
        semop(sem_id, &sops, 1);
        get_time();
        printf("Client %d enters barbershop.\n", client_pid);
        fflush(stdout);
        int helper = push(waiting_hallway, client_pid);
        sops.sem_op = 1;
        semop(sem_id, &sops, 1);

        if(helper >= 0){
          get_time();
          printf("Client %d has taken place in queue\n", client_pid);
          fflush(stdout);
        }
        else{
          get_time();
          printf("Client %d couldn't take a spot in queue, leaves the shop\n", client_pid);
          continue;
        }

        sops.sem_num = 2;
        sops.sem_op = -1;
        semop(sem_id, &sops, 1);
        int status = semctl(sem_id, 0, GETVAL);
        if(status == 1 && helper == 0){
          ///wakey time
          get_time();
          printf("Client %d wakes the barber up\n", client_pid);
          sops.sem_num = 0;
          sops.sem_op = -1;
          semop(sem_id, &sops, 1);
        }
        sops.sem_num = 2;
        sops.sem_op = 1;
        semop(sem_id, &sops, 1); /// zwolnienie statusu budzenia
        /// waiting for cutting to end before proceeding
        cuts_amount++;
        get_time();
        printf("Client %d number of cuts : %d, leaves the shop\n", client_pid, cuts_amount);
      }
      exit(0);
    }
    if(client_pid > 0){
      children[i] = client_pid;
    }
    if(client_pid < 0){
      printf("Problem with fork occured\n");
    }
  }
  while((waitpid = wait(&status)) > 0);
  for(i = 0; i < N; i++){
    kill(children[i], SIGINT);
  }
  return 0;
}
