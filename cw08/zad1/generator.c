#include <stdlib.h>
#include <stdio.h>
#include <time.h>

size_t BUFFERSIZE = 1024;


int main(int argc, char** argv){

  if(argc != 3){
    printf("Expected 2 arguments: [file_handle] [record_no]\n");
    exit(1);
  }

  char* file_handle = argv[1];
  int record_no = atoi(argv[2]);
  printf("%s %d\n",file_handle, record_no);

  FILE* file = fopen(file_handle,"w+");
  srand(time(NULL));

  int start = 0;
  char record[BUFFERSIZE];

  for(int i = 0; i < record_no ; i++){
    start = 0;
    start = snprintf(record, BUFFERSIZE, "%d.", i);
    for(int j = start; j < BUFFERSIZE-1 ; j++){
      record[j] = 97 + rand()%26;
    }
    record[BUFFERSIZE-1] = '\n';
    fwrite(record, 1, BUFFERSIZE, file);
  }

  fclose(file);
  return 0;
}
