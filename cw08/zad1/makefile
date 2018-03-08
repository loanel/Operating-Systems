CC = gcc
CFLAGS = -Wall

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%: %.o
	$(CC) $(CFLAGS) $^ -o $@


all:
	$(CC) $(CFLAGS) zad1a.c -lpthread -o zad1a
	$(CC) $(CFLAGS) zad1b.c -lpthread -o zad1b
	$(CC) $(CFLAGS) zad1c.c -lpthread -o zad1c
	$(CC) $(CFLAGS) generator.c -o generator

.PHONY: clean

clean:
	-rm -rf *.o zad1a
	-rm -rf *.o zad1b
	-rm -rf *.o zad1c
	-rm -rf *.o generator
