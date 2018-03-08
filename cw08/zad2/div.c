#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

void* run(void* unused) {
	while (1) {
	}
	return NULL;
}

void* division(void* unused) {
    printf ("Trying to divide by 0\n");
    int what = 1/0;
    return NULL;
}

int main(int argc, char** argv) {

	pthread_t thread;
	if(pthread_create(&thread, NULL, run, NULL) < 0){
    perror("Failed to create thread:\n");
    exit(1);
  }
  /// Waiting 1 second for clarity
  sleep(1);

	pthread_t dividing_thread;
	if(pthread_create(&dividing_thread, NULL, division, NULL) < 0){
    perror("Failed to create dividing thread:\n");
    exit(1);
  }

	sleep(1);

	if(pthread_join(thread, NULL) < 0){
    perror("Failed to join thread 1:\n");
    exit(1);
  }

  if(pthread_join(dividing_thread, NULL) < 0){
    perror("Failed to join thread 2:\n");
    exit(1);
  }
	return 0;
}
