CC = gcc
CFLAGS = -Wall

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%: %.o
	$(CC) $(CFLAGS) $^ -o $@


all:
	$(CC) $(CFLAGS) server.c -lpthread -o server
	$(CC) $(CFLAGS) client.c -lpthread -o client

.PHONY: clean

clean:
	-rm -rf *.o server
	-rm -rf *.o client
