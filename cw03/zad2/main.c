#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

int intConversion(char *number){
  int convertedNumber = 0;
  for(int i = 0; i < strlen(number); i++){
    convertedNumber = convertedNumber * 10 + (number[i] - 48);
  }
  return convertedNumber;
}

void parseArguments(int argc, char **argv){
	if(argc != 4){
		printf("Wrong number of arguments - the arguments should be <batchfile> <cputime_in_seconds> <size_in_megabytes>");
		exit(1);
	}
	FILE *batchFile = fopen(argv[1], "r");
	if(batchFile == NULL){
		perror("Failed to open batchfile:");
		exit(1);
	}
	fclose(batchFile);
}

void setEnvironment(char *command){

	char *varName = strtok(command, "\t\n ");
	char *varValue = strtok(NULL, "\t\n ");   /// has to be like this because strtok keeps data in static variables, null makes it use stored data
	if(varValue == NULL){//delete variable from environment
		if(unsetenv(varName) == -1){
			perror("Error while unsetting environment:");
			exit(1);
		}
		printf("Removed environment variable %s\n", command);
	}
	else{
		if(setenv(varName, varValue, 1) == -1){
			perror("Error while setting environment:");
			exit(1);
		}
		printf("Added environment variable %s\n", command);
	}
}

void runBatchScript(char *command, rlim_t cpu, rlim_t memory){
	#define script_arguments 32

	char **argHolder = malloc(sizeof(char *) * script_arguments);
	int argCounter = 0;
	argHolder[0] = strtok(command, "\t\n ");
	argCounter++;
	char *copyHelper = strtok(NULL, "\t\n ");
	while(copyHelper != NULL){
		if(argCounter >= script_arguments){
			printf("The maximum amount of arguments for a batch command in this program is 32");
			exit(1);
		}
		else{
			argHolder[argCounter] = copyHelper;
			copyHelper = strtok(NULL, "\t\n ");
			argCounter++;
		}
	}
	/// filled argHolder properly in order to use exec, begging forking operations
	pid_t pid = fork();
	if(pid == 0){ // child
		/// task2 modifications : set rlimit for cpu and memory
		struct rlimit cpuLimiter;
		struct rlimit memLimiter;
		cpuLimiter.rlim_cur = cpu;
		cpuLimiter.rlim_max = cpu;
		memLimiter.rlim_cur = memory*1024*1024;
		memLimiter.rlim_max = memory*1024*1024;
		/// setting limits
		if(setrlimit(RLIMIT_CPU, &cpuLimiter) == -1){
			perror("Error while setting cpu limit :");
			exit(1);
		}
		if(setrlimit(RLIMIT_AS, &memLimiter) == -1){ // adress space
			perror("Error while setting mem limit :");
			exit(1);
		}
		/// limits set, execute
		if(execvp(argHolder[0], argHolder) == -1){
			perror("Error while execvp:");
			exit(1);
		}
	}
	else{
			if(pid > 0){//parent
				int childStatus;
				struct rusage timeCounter;
				/// using wait3 instead, easiest way to count time for a forked process, wait 3 == waitpid(-1, childStatus, 0), then getrusage with RUSAGE_CHILDREN
				wait3(&childStatus, 0, &timeCounter); // equal to waitpid(-1, &childStatus, 0)
				if(WIFEXITED(childStatus) && WEXITSTATUS(childStatus) != 0){
					printf("An error occured in the child process %s\n", command);
					exit(1);
				}
				else{
					printf("The process used %fs user time and %fs system time\n",
									(float)(timeCounter.ru_utime.tv_sec + (float)timeCounter.ru_utime.tv_usec/1000000),
									(float)(timeCounter.ru_stime.tv_sec + (float)timeCounter.ru_stime.tv_usec/1000000));
				}
			}
			else{
				perror("Error while trying to fork :");
				exit(1);
			}
		}
	/// cleanup

	#undef script_arguments
}

void runBatchCommands(char *batchFileName, rlim_t cpu, rlim_t memory){

	FILE *batchFile = fopen(batchFileName, "r");
	size_t length = 32;
	char *readHelper = calloc(length, sizeof(char));
	while(getline(&readHelper, &length, batchFile) != -1){
		printf("~~~~~~~");
		printf("Running %s", readHelper);
		if(readHelper[0] == '#'){// enviromental variable
			setEnvironment(readHelper + 1); // in order to get rid of #
		}
		else{
			runBatchScript(readHelper, cpu, memory);
		}
	}
	free(readHelper);
	fclose(batchFile);

}
int main(int argc, char** argv) {

	parseArguments(argc, argv);
	runBatchCommands(argv[1], (rlim_t)intConversion(argv[2]), (rlim_t)intConversion(argv[3]));

}
