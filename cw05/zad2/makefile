CC = gcc

all : master slave
master:
	gcc -Wall master.c -o master
slave:
	gcc -Wall slave.c -lm -o slave
clean:
	rm -f master
	rm -f slave
	rm -r data
