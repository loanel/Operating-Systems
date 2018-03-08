#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
/// using constant double instead of define to have the type set
const double Re_Maximum = 1.0;
const double Re_Minimum = -2.0;
const double Im_Maximum = 1.0;
const double Im_Minimum = -1.0;

int measureIterations(double Re, double Im, int K){
  /// everything according to mandelbrot set rules
  double complex c = Re + Im * I; // as in I = sqrt(-1)
  double complex z = 0.0;
  int i = 0;
	while(i < K && cabs(z) < 2.0){
		z = cpow(z, 2.0) + c;
		i++;
	}
  return i;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
	char *path = argv[1];
  const int N = atoi(argv[2]);
  const int K = atoi(argv[3]);
  int fifoPipe = open(path, O_WRONLY);
  if(fifoPipe == -1) {
      perror("Unable to open fifo pipe file\n");
      exit(1);
  }
  double Re,Im;
  int iters;
  char *output = malloc(100 * sizeof(char));
  for(int j = 0; j < N; j++) {
    Re = ((double)rand()/(double)RAND_MAX) * (Re_Maximum - Re_Minimum) + Re_Minimum;
    Im = ((double)rand()/(double)RAND_MAX) * (Im_Maximum - Im_Minimum) + Im_Minimum;
    if(Re == 1) Re = 0;
    if(Im == 1) Re = 0;
    iters = measureIterations(Re, Im, K);
    /// print to output like in earlier tasks
    sprintf(output, "%lf %lf %d\n", Re, Im, iters);
    write(fifoPipe, output, 100);
  }
  close(fifoPipe);
  free(output);
  return 0;
}
