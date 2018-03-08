#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

int intConversion(char *number){
  int convertedNumber = 0;
  for(int i = 0; i < strlen(number); i++){
    convertedNumber = convertedNumber * 10 + (number[i] - 48);
  }
  return convertedNumber;
}

void beginSlavery(int slave_amount, char *path){
	char *commands[5] = {"slave",path,"2000000","100"};
    for(int j = 0; j < slave_amount ;j++) {
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            perror("Failed to fork the process\n");
            exit(1);
        }
        if (pid == 0) {
            if (execv(commands[0], commands) == -1) {
                perror("An error has occured while trying to execv : ");
                exit(1);
            }
        }
    }
}

void retrieveData(char *path, int **T, int R){
  /// reading easier using sys version
  int readHelper = open(path, O_RDONLY);
  if(readHelper  == -1){
      perror("Failed to open fifo file\n");
      exit(1);
  }
  char *buffer = malloc(100*sizeof(char));
  double Re,Im;
  int iters;
	int readChecker = read(readHelper, buffer, 100);
  while(readChecker > 0){
      Re = atof(strtok(buffer," "));
      Im = atof(strtok(NULL," "));
      iters = atoi(strtok(NULL," \n")); /// so after all there is a function that converts strings to integers...
      /// creating coordinates, have to add minus portions, it will simply move the graph by factors up and right
      int x = (int)(((Re+2)/3)*R);
      int y = (int)(((Im+1)/2)*R);
      if(x == R) x--;
      if(y == R) y--;
      T[x][y] = iters;
      readChecker = read(readHelper, buffer, 100);
    }
	if(readChecker == -1){
		perror("Error during reading from fifo file\n");
		exit(1);
	}
  close(readHelper);
  free(buffer);
}

void storeData(int **T, int R){
  /// writing easier using lib version
  FILE *dataStorage = fopen("data", "w");
  if(dataStorage == NULL) {
    printf("Unable to open/create file with data\n");
    exit(1);
  }
  for(int i = 0; i < R ; i++){
    for(int j = 0; j < R; j++){
      fprintf (dataStorage, "%d %d %d\n", i, j, T[i][j]);
    }
  }
  fclose(dataStorage);
}

void plotSet(int R){
  FILE *gnu = popen("gnuplot", "w");
	if(gnu == NULL){
      printf ("gnuplot popen error\n");
      exit(1);
  }
	/// according to lab instruction ...
  fprintf(gnu, "set view map\n");
	fprintf(gnu, "set xrange [0:%d]\n",R);
	fprintf(gnu, "set yrange [0:%d]\n",R);
  fprintf(gnu, "plot 'data' with image\n");
  fflush(gnu);
  getc(stdin);
	fprintf(gnu, "exit\n");
  pclose(gnu);
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
      printf("%s\n", "Wrong input arguments. The proper input is <named_pipe> <array_size>");
      exit(1);
  }
  char *path = argv[1];
	const int R = intConversion(argv[2]);
  /// creating name pipe as fifo, with 0666 - all users can read and write but cannot execute
  if(mkfifo(path, 0666) == -1){
      printf("Error while creating the fifo pie\n");
      exit(1);
  }
  /// initialize table of R size
  int **T = malloc(R * sizeof(int *));
  for(int i = 0; i < R ; i++){
		T[i] = calloc(R, sizeof(int));
  }
	/// run code modules
  /// how many slaves should write to the fifo pipe, default set to 3
  beginSlavery(3, path);
	sleep(3);
  retrieveData(path, T, R);
	storeData(T, R);
  plotSet(R);

  for(int i = 0; i < R; i++){
	 free(T[i]);
  }
  free(T);
	unlink(path);

  return 0;
}
