#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define _XOPEN_SOURCE 500
int direction = 1;

void changeSign(){
	direction = direction * (-1);
}

void destroy(){
	exit(0);
}

int main() {
	
	struct sigaction signal_action;
	signal_action.sa_handler = destroy;
	signal_action.sa_flags = 0;

	signal(SIGTSTP, changeSign);
	sigaction(SIGINT, &signal_action, NULL);

	int iterator = 0;
	while( 1 ){
		usleep(90000);
		printf("%c", 'A'+iterator);
		fflush(stdout);
		iterator += direction;
		if(iterator<0) iterator= 'Z'-'A';
		if(iterator>'Z'-'A') iterator= 0;
	}
	
	return 0;
		
}
