#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong number of arguments - the only argument should be the name of the environmental variable");
        exit(1);
    }
    char *environmentalVariable = getenv(argv[1]);
    if (environmentalVariable == NULL) {
        printf("Couldn't find the variable : %s\n", argv[1]);
    } else {
        printf("%s\n", environmentalVariable);
    }
    return 0;
}
