#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

// Zgodnie z poleceniem :
//1 - wysłanie sygnału do procesu, gdy żaden wątek nie ma zamaskowanego tego sygnału
//2 - wysłanie sygnału do procesu, gdy główny wątek programu ma zamaskowany ten sygnał, a wszystkie pozostałe wątki nie,
//3 - wysłanie sygnału do procesu, gdy wszystkie wątki mają zainstalowaną niestandardową procedurę obsługi przerwania, która wypisuje informację o nadejściu tego sygnału oraz PID i TID danego wątku
//4 - wysłanie sygnału do wątku z zamaskowanym tym sygnałem
//5 - wysłanie sygnału do wątku, w którym zmieniona jest procedura obsługi sygnału, jak przedstawiono w punkcie 3
#define MODE 1
// SIGUSR1, SIGTERM, SIGKILL, SIGSTOP
#define SIGNAL SIGSTOP

void handler(int signo) {
	printf("PID - %d TID - %li\n", getpid(), pthread_self());
}

void* run(void* unused) {
	if(MODE == 5){
    signal(SIGNAL, handler);
  }

	if(MODE == 4){
    sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGNAL);
    if(sigprocmask(SIG_SETMASK, &set, NULL) < 0){
      perror("Failed to set mask\n");
      exit(1);
    }
    printf("Thread masking signal\n");
	}
	while(1){
      /// work simulation
  }
	return NULL;
}

int main(int argc, char ** argv) {

	if(MODE == 2){
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGNAL);
		if(sigprocmask(SIG_SETMASK, &set, NULL) < 0){
      perror("Failed to set mask:\n");
      exit(1);
    }
		printf("Set mask of %d at main\n", SIGNAL);
	}

	pthread_t thread;

	if(pthread_create(&thread, NULL, run, NULL) < 0){
    perror("Failed to create thread:\n");
  }
	printf("Thread created - waiting for 1 second for clarity\n");

	sleep(1);

	if(MODE == 4 || MODE == 5){
  	if(pthread_kill(thread, SIGNAL) < 0){
      perror("Failed to kill thread:\n");
      exit(1);
    }
    printf("Sent signal to thread.\n");
  }
  else{
    if(MODE == 3) {
      signal(SIGNAL, handler);
    }
    if(raise(SIGNAL) < 0){
    perror("Failed to raise signal: \n");
    }

    printf("Sent signal to main\n");
  }

	if(pthread_join(thread, NULL) < 0){
    perror("Failed to join threads\n");
  }

  printf("Program ended without errors\n");

	return 0;
}
