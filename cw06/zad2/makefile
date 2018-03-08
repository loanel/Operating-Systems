CC = gcc


all : service user
service:
	gcc -Wall service.c -lrt -o service
user:
	gcc -Wall user.c -lrt -o user
clean:
	rm -f service
	rm -f user
