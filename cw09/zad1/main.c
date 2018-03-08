#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

int number_array[32]={0};
/// readers-preference version, we have to count the readers
int cur_readers = 0;

sem_t resource_semaphore;
sem_t readers_semaphore;

/// making a structure for arguments that readers are created with
typedef struct t_attr{
  int value;
  /// and some id holder so we can diffrentiate threads
  int identificator;
} t_attr;


void signal_handler(int signum){
  if(signum == SIGINT){
		sem_destroy(&resource_semaphore);
		sem_destroy(&readers_semaphore);
		exit(0);
	}
}

void *writer_procedure(void *args){
  t_attr *attributes = (t_attr*)args;
  int id = attributes->identificator;

  /// writer job
  while(1){
    /// wait for access to resource, then take it
    sem_wait(&resource_semaphore);
    ///CRITICAL SECTION
    int to_edit = rand()%32;
    int a_id;
    int value;
    for(int i = 0; i < to_edit; i++){
      a_id = rand()%32;
      value = rand()%64;
      number_array[a_id] = value;
      printf("Writer %d managed to write %d in number_holder[%d]\n", id, value, a_id);
    }
    printf("\n");
    fflush(stdout);
    /// free resource
    sem_post(&resource_semaphore);
    ///fin
    ///sleep(3)
  }
  return NULL;
}

void *reader_procedure(void *args){
  t_attr *attributes = (t_attr*)args;
  int id = attributes->identificator;
  int value_to_check = attributes->value;
  while(1){
    /// CRITICAL SECTION
    /// get the semaphore for readers counter
    sem_wait(&readers_semaphore);
    cur_readers++;
    /// if he is the first reader, lock the resource for writing
    if(cur_readers == 1){
      sem_wait(&resource_semaphore);
    }
    /// free up readers counter semaphore
    sem_post(&readers_semaphore);
    ///fin

    int numbers_counter = 0;
    for(int i = 0; i < 32; i++){
      if(number_array[i] % value_to_check == 0){
        numbers_counter++;
        /// notify that you did something
        printf("Reader %d found that number_array[%d] mod %d == 0, increasing counter\n", id, i, value_to_check);
      }
    }
    printf("Reader %d found %d numbers that are divisible by %d\n", id, numbers_counter, value_to_check);
    fflush(stdout);

    /// CRITICAL SECTION
    /// get the readers semaphore
    sem_wait(&readers_semaphore);
    /// finished reading - decrease amount of readers
    cur_readers--;
    /// if nobodys reading - let the writer do some work
    if(cur_readers == 0){
      sem_post(&resource_semaphore);
    }
    sem_post(&readers_semaphore);
    ///fin
    //sleep(3)

  }
  return NULL;
}

int main(int argc, char **argv){

  int readers;
  int writers;

  if(argc != 3){
    printf("Wrong number of arguments, the arguments are : [writers] [readers]");
    exit(0);
  }

  writers = atoi(argv[1]);
  readers = atoi(argv[2]);
  if(writers < 0 || readers < 0){
    printf("The writers and readers number has to be a positive number");
    exit(0);
  }
  sem_init(&resource_semaphore, 0, 1);
  sem_init(&readers_semaphore, 0, 1);
  signal(SIGINT, signal_handler);

  /// initializing readers and writers arrays
  /// arrays for threads
  pthread_t *writer_holder = malloc(sizeof(pthread_t)*writers);
  pthread_t *reader_holder = malloc(sizeof(pthread_t)*readers);
  /// arrays for thread attributes
  t_attr **writer_attr = malloc(sizeof(t_attr*)*writers);
  t_attr **reader_attr = malloc(sizeof(t_attr*)*readers);

  for(int i = 0; i < writers; i++){
    writer_attr[i] = malloc(sizeof(t_attr));
  }

  for(int i = 0; i < readers; i++){
    reader_attr[i] = malloc(sizeof(t_attr));
  }

  ///creating threads
  for(int i = 0; i < writers; i++){
    writer_attr[i]->identificator = i;
    pthread_create(&writer_holder[i], NULL, writer_procedure, writer_attr[i]);
  }

  for(int i = 0; i < readers; i++){
    reader_attr[i]->identificator = i;
    reader_attr[i]->value = rand()%64;
    pthread_create(&reader_holder[i], NULL, reader_procedure, reader_attr[i]);
  }

  /// joining threads

  for(int i = 0; i < writers; i++){
    pthread_join(writer_holder[i], NULL);
  }

  for(int i = 0; i < readers; i++){
    pthread_join(reader_holder[i], NULL);
  }
  ///cleanup
  sem_destroy(&resource_semaphore);
  sem_destroy(&readers_semaphore);

  return 0;
}
