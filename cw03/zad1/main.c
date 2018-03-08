#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void parseArguments(int argc, char **argv){
	if(argc != 2){
		printf("Wrong number of arguments - the only argument should be the path to batchfile");
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

void runBatchScript(char *command){
	#define script_arguments 32
/*
	char *argHolder[script_arguments];
	memset(argHolder, 0, (script_arguments) * sizeof(char *));  /// easiest way to fill two dimensional array
*/
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
		if(execvp(argHolder[0], argHolder) == -1){
			perror("Error while execvp:");
			exit(1);
		}
	}
	else{
			if(pid > 0){//parent
				int childStatus;
				wait(&childStatus); // equal to waitpid(-1, &childStatus, 0)
				if(WIFEXITED(childStatus) && WEXITSTATUS(childStatus) != 0){
					printf("An error occured in the child process %s", command);
					exit(1);
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

void runBatchCommands(char *batchFileName){

	FILE *batchFile = fopen(batchFileName, "r");
	size_t length = 32;
	char *readHelper = calloc(length, sizeof(char));
	while(getline(&readHelper, &length, batchFile) != -1){
		printf("Running %s", readHelper);
		if(readHelper[0] == '#'){// enviromental variable
			setEnvironment(readHelper + 1); // in order to get rid of #
		}
		else{
			runBatchScript(readHelper);
		}
	}
	free(readHelper);
	fclose(batchFile);

}
int main(int argc, char** argv) {

	parseArguments(argc, argv);
	runBatchCommands(argv[1]);

}
