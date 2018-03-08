CC = gcc

all : main testCode runtest
main:
	gcc -Wall main.c -o main

testCode:
	gcc -Wall testCode.c -o testCode

runtest:
	./main batchCommands

.PHONY: clean
clean:
	rm -f main
	rm -f testCode
