#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

int main(){

#define allocations 100000

char **allocator = malloc(sizeof(char*) *allocations);
for(int i = 0; i < allocations; i++){
  if((allocator[i] = malloc(1048576 * sizeof(char)))!=NULL){
    printf("Allocation number %d\n", i + 1);
    }
}
for(int j=0; j<allocations; j++){
  free(allocator[j]);
}

free(allocator);
#undef allocations

return 0;
}
