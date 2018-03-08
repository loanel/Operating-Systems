#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

int number_array[256]={0};
/// make them global, useful for a lot of things
int readers;
int writers;
pthread_t *writer_holder;
pthread_t *reader_holder;
/// doing fifo version with mutex, we will need 2 arrays, one for threads which will act as 'fifo', one for conditions
pthread_cond_t *conditions_holder;
int *fifo;
///mutex
pthread_mutex_t mutex;

/// making a structure for arguments that readers are created with
typedef struct t_attr{
  int value;
  /// and some id holder so we can diffrentiate threads
  int identificator;
} t_attr;


void signal_handler(int signum){
  if(signum == SIGINT){
		for(int i = 0; i < writers; i++) pthread_cancel(writer_holder[i]);
    for(int i = 0; i < readers; i++) pthread_cancel(reader_holder[i]);
    for(int i = 0; i < writers+readers; i++) pthread_cond_destroy(&conditions_holder[i]);
    pthread_mutex_destroy(&mutex);
		exit(0);
	}
}

void *writer_procedure(void *args){
  t_attr *attributes = (t_attr*)args;
  int id = attributes->identificator;
  int queue_position;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

  /// writer job
  while(1){
    queue_position = -1;
    /// take position in queue
    for(int i = 0; i < writers+readers; i++){
      if(fifo[i] == 0){
        queue_position = i;
        fifo[i] = 1;
        break;
      }
    }
    if(queue_position == -1) continue;

    /// CRITICAL SECTION
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&conditions_holder[queue_position], &mutex);

    int to_edit = rand()%256;
    int a_id;
    int value;
    for(int i = 0; i < to_edit; i++){
      a_id = rand()%256;
      value = rand()%1024;
      number_array[a_id] = value;
      printf("Writer %d managed to write %d in number_holder[%d]\n", id, value, a_id);
    }
    printf("\n");
    fflush(stdout);
    pthread_cond_broadcast(&conditions_holder[queue_position]);
    fifo[queue_position] = 0;
    pthread_mutex_unlock(&mutex);
    ///fin
    ///sleep(3)
  }
  return NULL;
}

void *reader_procedure(void *args){
  t_attr *attributes = (t_attr*)args;
  int id = attributes->identificator;
  int value_to_check = attributes->value;
  int queue_position;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  while(1){
    queue_position = -1;
    /// take position in queue
    for(int i = 0; i < writers+readers; i++){
      if(fifo[i] == 0){
        queue_position = i;
        fifo[i] = 1;
        break;
      }
    }
    if(queue_position == -1) continue;
    /// CRITICAL SECTION
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&conditions_holder[queue_position], &mutex);

    int numbers_counter = 0;
    for(int i = 0; i < 256; i++){
      if(number_array[i] % value_to_check == 0){
        numbers_counter++;
        /// notify that you did something
        printf("Reader %d found that number_array[%d] mod %d == 0, increasing counter\n", id, i, value_to_check);
      }
    }
    printf("Reader %d found %d numbers that are divisible by %d\n", id, numbers_counter, value_to_check);
    fflush(stdout);
    pthread_cond_broadcast(&conditions_holder[queue_position]);
    fifo[queue_position] = 0;
    pthread_mutex_unlock(&mutex);
    ///fin
    //sleep(3)

  }
  return NULL;
}

int main(int argc, char **argv){

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
  signal(SIGINT, signal_handler);

  /// initializing readers and writers arrays
  /// arrays for threads
  writer_holder = malloc(sizeof(pthread_t)*writers);
  reader_holder = malloc(sizeof(pthread_t)*readers);
  /// arrays for thread attributes
  t_attr **writer_attr = malloc(sizeof(t_attr*)*writers);
  t_attr **reader_attr = malloc(sizeof(t_attr*)*readers);

  for(int i = 0; i < writers; i++){
    writer_attr[i] = malloc(sizeof(t_attr));
  }

  for(int i = 0; i < readers; i++){
    reader_attr[i] = malloc(sizeof(t_attr));
  }

  /// creating conditions and the fifo, using calloc here for safety
  conditions_holder = calloc(writers + readers, sizeof(pthread_cond_t));
  for(int i = 0; i < writers + readers; i++){
    pthread_cond_init(&conditions_holder[i], NULL);
  }
  fifo = calloc(writers + readers, sizeof(int));

  ///init mutex
  pthread_mutex_init(&mutex, NULL);

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

  /// looping over fifo for an eternity, we wont even get to joins
  int loop_index = 0;
  while(1){
    if(fifo[loop_index]!=0){
      /// give the green light to thread in 'fifo' at that certain position
      /// this isn't a normal fifo, but it does prevent choking to some degree
      pthread_cond_broadcast(&conditions_holder[loop_index]);
    }
    loop_index = (loop_index + 1) % (writers + readers);
  }


  /// joining threads, we won't even get here, but in case something strange happens...
  for(int i = 0; i < writers; i++){
    pthread_join(writer_holder[i], NULL);
  }

  for(int i = 0; i < readers; i++){
    pthread_join(reader_holder[i], NULL);
  }


  return 0;
}
