CC = gcc

all : main cpuTest memTest testCode runtest
main:
	gcc -Wall main.c -o main

testCode:
	gcc -Wall testCode.c -o testCode

cpuTest:
	gcc -Wall cpuTest.c -o cpuTest

memTest:
	gcc -Wall memTest.c -o memTest

runtest:
	./main batchCommands 10 20

runfail:
	./main batchCommands 10 10

totaltest:
	./main globalCommands 10 20
.PHONY: clean
clean:
	rm -f main
	rm -f testCode
	rm -f memTest
	rm -f cpuTest
