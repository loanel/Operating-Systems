#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fifoq.h"

int N;
fifo_queue *waiting_hallway;
int memory_id;
/// semaphores, 3 required
sem_t *barber_semaphore;
sem_t *fifo_semaphore;
sem_t *status_semaphore;

void sigint_handler(int sig){
  exit(0);
}

void barber_closes(){
  printf("\n");
  get_time();

  sem_close(barber_semaphore);
  sem_unlink(BARBER);

  sem_close(fifo_semaphore);
  sem_unlink(FIFO);

  sem_close(status_semaphore);
  sem_unlink(BARBER_STATUS);

  if(munmap(waiting_hallway, sizeof(waiting_hallway)) == -1){
    printf("Problem with unlinking shared memory\n");
  }
  if(shm_unlink(SHARED_MEM) == -1){
    printf("Problem with unlinking shared memory\n");
  }
}

void create_semaphores(){
  memory_id = shm_open(SHARED_MEM, O_CREAT | O_EXCL | O_RDWR, 0666); /// shared memory with all permissions
  if(memory_id == -1){
    perror("Barber didn't create shared memory properly");
    exit(1);
  }
  if(ftruncate(memory_id, sizeof(fifo_queue)) == -1){
    printf("Barber didn't create shared memory properly");
    exit(1);
  }
  void *void_ptr = mmap(NULL, sizeof(fifo_queue), PROT_WRITE | PROT_READ, MAP_SHARED, memory_id, 0);

  if(void_ptr == (void*)(-1)){
    printf("Barber didn't attach the shared memory");
    exit(1);
  }
  waiting_hallway = (fifo_queue *) void_ptr;

  barber_semaphore = sem_open(BARBER, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
  if(barber_semaphore == SEM_FAILED){
    printf("Barber didn't initialize semaphore number 1\n");
    exit(1);
  }

  fifo_semaphore = sem_open(FIFO, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
  if(fifo_semaphore == SEM_FAILED){
    printf("Barber didn't initialize semaphore number 2\n");
    exit(1);
  }

  status_semaphore = sem_open(BARBER_STATUS, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
  if(status_semaphore == SEM_FAILED){
    printf("Barber didn't initialize semaphore number 3\n");
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
    sem_wait(fifo_semaphore);
    pid_t pid_to_cut = pop(waiting_hallway);
    sem_post(fifo_semaphore);

    if(pid_to_cut > 0){
      get_time();
      printf("Barber cut %d\n", pid_to_cut);
      fflush(stdout);
      kill(pid_to_cut, SIGRTMIN);
    }
    else{
      sem_post(barber_semaphore);
      get_time();
      printf("Barber goes to sleep.\n");

      int status;
      sem_getvalue(barber_semaphore, &status);
      while(status != 0){
        sem_getvalue(barber_semaphore, &status);
      }
      get_time();
      printf("Barber woke up.\n");
    }
  }
  return 0;
}
