#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "fifoq.h"

int N;
int S;
fifo_queue *waiting_hallway;
int memory_id;
int cuts_amount = 0;
pid_t *children;
int is_cut_flag = 0;
/// semaphores, 3 required
sem_t *barber_semaphore;
sem_t *fifo_semaphore;
sem_t *status_semaphore;

void sigint_handler(int sig){
  exit(0);
}

void got_cut(int sig){
  is_cut_flag = 1;
  return;
}

void client_leaves(){
  get_time();
  printf("Client %d ending\n", getpid());
  if(sem_close(barber_semaphore) == -1)
		printf("Semaphore 1 close failed.\n");
	if(sem_close(fifo_semaphore) == -1)
		printf("Semaphore 2 close failed.\n");
	if(sem_close(status_semaphore) == -1)
		printf("Semaphore 3 close failed.\n");
	if(munmap(waiting_hallway, sizeof(waiting_hallway))== -1)
		printf("Couldn't unlink shared memory.\n");
}

void create_semaphores(){
  memory_id = shm_open(SHARED_MEM, O_RDWR, 0666); /// shared memory with all permissions
  if(memory_id == -1){
    perror("Barber didn't create shared memory properly");
    exit(1);
  }

  void *void_ptr = mmap(NULL, sizeof(fifo_queue), PROT_WRITE | PROT_READ, MAP_SHARED, memory_id, 0);
  if(void_ptr == (void*)(-1)){
    printf("Barber didn't attach the shared memory");
    exit(1);
  }
  waiting_hallway = (fifo_queue *) void_ptr;

  barber_semaphore = sem_open(BARBER, O_RDWR);
  if(barber_semaphore == SEM_FAILED){
    printf("Barber didn't initialize semaphore number 1\n");
    exit(1);
  }

  fifo_semaphore = sem_open(FIFO, O_RDWR);
  if(fifo_semaphore == SEM_FAILED){
    printf("Barber didn't initialize semaphore number 2\n");
    exit(1);
  }

  status_semaphore = sem_open(BARBER_STATUS, O_RDWR);
  if(status_semaphore == SEM_FAILED){
    printf("Barber didn't initialize semaphore number 3\n");
    exit(1);
  }
}

int main(int argc, char **argv){
  atexit(client_leaves);
  signal(SIGINT, sigint_handler);
  signal(SIGRTMIN, got_cut);
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

        sem_wait(fifo_semaphore);
        get_time();
        printf("Client %d enters barbershop.\n", client_pid);
        fflush(stdout);
        int helper = push(waiting_hallway, client_pid);
        sem_post(fifo_semaphore);

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

        sem_wait(status_semaphore);
        int status;
        sem_getvalue(barber_semaphore, &status);
        if(status == 1 && helper == 0){
          ///wakey time
          get_time();
          printf("Client %d wakes the barber up\n", client_pid);
          sem_wait(barber_semaphore);
        }
        sem_post(status_semaphore);
        /// waiting for cutting to end before proceeding
        while(is_cut_flag == 0);
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
