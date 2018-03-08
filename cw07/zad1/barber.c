#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
fifo_queue *waiting_hallway;
int sem_id = -1;
int memory_id = -1;

void sigint_handler(int sig){
  exit(0);
}

void barber_closes(){
  shmdt(waiting_hallway); //detach
  shmctl(memory_id, IPC_RMID, NULL); /// mark the segment to be destroyed
  semctl(sem_id, 0, IPC_RMID); /// remove semaphores, awaken all processes blocked by semop
}

void create_semaphores(){
  key_t key = ftok(getenv("HOME"), ID);
  if(key == -1){
    printf("Barber didn't recieve key\n");
    exit(1);
  }
  memory_id = shmget(key, sizeof(fifo_queue), IPC_CREAT | IPC_EXCL | 0666); /// shared memory with all permissions
  if(memory_id == -1){
    perror("Barber didn't create shared memory properly");
    exit(1);
  }

  void *void_ptr = (fifo_queue *) shmat(memory_id, NULL, 0);
  if(void_ptr == (void*)(-1)){
    printf("Barber didn't attach the shared memory");
    exit(1);
  }
  printf("A");
  waiting_hallway = (fifo_queue *) void_ptr;

  sem_id = semget(key, 3, IPC_CREAT | IPC_EXCL | 0666);
  if(sem_id == -1){
    perror("Barber didn't create the semaphores\n");
    exit(1);
  }
  /// setting up 3 semaphores
  /// barber
  if(semctl(sem_id, 0, SETVAL, 0) == -1){
    printf("Barber didn't initialize the semaphore\n");
    exit(1);
  }
  /// fifo_queue
  if(semctl(sem_id, 1, SETVAL, 1) == -1){
    printf("Barber didn't initialize the semaphore\n");
    exit(1);
  }
  /// barber wakeup status
  if(semctl(sem_id, 2, SETVAL, 1) == -1){
    printf("Barber didn't initialize the semaphore\n");
    exit(1);
  }

}

int main(int argc, char **argv){
  atexit(barber_closes);
  signal(SIGINT, sigint_handler);
  if(argc != 2){
    printf("Wrong number of arguments - the required amount is 1. Proper format - ./barber  [waiting room length]");
    return 1;
  }
  N = atoi(argv[1]);
  if(N <= 0){
    printf("The program arguments have to be positive integers.");
    return 1;
  }
  printf("N = %d\n", N);
  create_semaphores();
  waiting_hallway = initialization(waiting_hallway, N);

  while(1){
    struct sembuf sops;
    sops.sem_num = 1; /// 1 - fifo semaphore
    sops.sem_op = -1; /// lock
    sops.sem_flg = 0;
    semop(sem_id, &sops, 1);
    pid_t pid_to_cut = pop(waiting_hallway);
    sops.sem_num = 1;
    sops.sem_op = 1; // unlock
    semop(sem_id, &sops, 1);
    if(pid_to_cut > 0){
      get_time();
      printf("Barber cut %d\n", pid_to_cut);
      fflush(stdout);
    }
    else{
      sops.sem_num = 0; /// barber semaphore
      sops.sem_op = 1; ///lock
      semop(sem_id, &sops, 1);
      get_time();
      printf("Barber goes to sleep.\n");
      sops.sem_num = 0;
      sops.sem_op = 0; /// wait for something to lower the semaphore to zero
      semop(sem_id, &sops, 1);
      get_time();
      printf("Barber woke up.\n");
    }
  }
  return 0;
}
