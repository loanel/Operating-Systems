CC = gcc
CFLAGS = -Wall

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%: %.o
	$(CC) $(CFLAGS) $^ -o $@


all:
	$(CC) $(CFLAGS) main.c -lpthread -o main

.PHONY: clean

clean:
	-rm -rf *.o main
