CC = gcc
CFLAGS = -Wall

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%: %.o
	$(CC) $(CFLAGS) $^ -o $@


all:
	$(CC) $(CFLAGS) zad2a.c -lpthread -o main
	$(CC) $(CFLAGS) div.c -lpthread -o div
.PHONY: clean

clean:
	-rm -rf *.o main
	-rm -rf *.o div
