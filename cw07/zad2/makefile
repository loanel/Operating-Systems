CC = gcc
CFLAGS = -Wall

all: fifoq.o
	ar rcs libfifoq.a fifoq.o
	$(CC) $(CFLAGS) barber.c -L. -lfifoq -lrt -lpthread -o barber
	$(CC) $(CFLAGS) client.c -L. -lfifoq -lrt -lpthread -o client

%.o: %.c
		$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean

clean:
	-rm -rf *.o barber
	-rm -rf *.o client
	-rm -rf *.o fifoq
	-rm libfifoq.a
