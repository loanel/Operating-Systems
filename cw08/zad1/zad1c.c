#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int thread_number;
char* file_handle;
int record_number;
char* wanted;
int file;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int BUFFERSIZE = 1024;


void* threadFunction(void* unused){
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  char** records_read = calloc(record_number, sizeof(char*));
  char* record_id = calloc(2, sizeof(char));
  int characters_read;
  int keep_working = 1;
  /// initialize space for records
  for(int i = 0; i < record_number; i++) {
    records_read[i] = calloc(1024, sizeof(char));
  }

  while(keep_working) {
    pthread_mutex_lock(&mutex);

    for(int i = 0; i < record_number; i++){
      if((characters_read = read(file, records_read[i], BUFFERSIZE)) == -1) {
        printf("Error at read(): %d: %s\n", errno, strerror(errno));
        exit(-1);
      }
    }
    /// reached end of file, cancel requirements met
    if(characters_read == 0) {
      for(int j = 0; j < thread_number; j++) {
        if(threads[j] != pthread_self()) {
          pthread_cancel(threads[j]);
        }
      }
      pthread_mutex_unlock(&mutex);
      break;
    }

    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < record_number; i++) {
      if(strstr(records_read[i], wanted) != NULL) {
        strncpy(record_id, records_read[i], 2);
        printf("%ld: found the word at record = %d\n", pthread_self(), atoi(record_id));
      }
    }
  }

  return NULL;
}

int main(int argc, char* argv[]) {
  file_handle = calloc(20, sizeof(char));
  wanted = calloc(10, sizeof(char));

  if(argc != 5) {
    printf("Wrong amount of arguments : the arguments are [nazwa programu] [ilosc watkow] [nazwa pliku] [ilosc rekordow] [szukane slowo]\n");
    exit(1);
  }
  ///setting variables
  thread_number = atoi(argv[1]);
  file_handle = argv[2];
  record_number = atoi(argv[3]);
  wanted = argv[4];
  threads = calloc(thread_number, sizeof(pthread_t));
  /// opening and testing file
  if((file = open(file_handle, O_RDONLY)) == -1) {
    printf("Error at open(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  for(int i = 0; i < thread_number; i++){

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(pthread_create(&threads[i], &attr, threadFunction, NULL) != 0) {
      printf("Error at pthread_create(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }
/*
  for(int i = 0; i < thread_number; i++) {
    pthread_join(threads[i], NULL);
  }
  */
  // for some reason when i use detached threads, strange things start happening
  // half the time no words are found
  // in 10-15% of the cases a bad file descriptor error happpens
  free(threads);
  close(file);

  return 0;
}
