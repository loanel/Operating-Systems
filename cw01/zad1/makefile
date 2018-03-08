CC = gcc

all: contactbook.o libcontactbook.a libcontactbook.so clean

libcontactbook.a: contactbook.o
	ar rcs $@ $^

libcontactbook.so: contactbook.c
	gcc -fPIC -shared $^ -o $@

contactbook.o: contactbook.c
	gcc -o $@ -c $^

.PHONY: clean
clean:
	rm -f *.o
