CC = gcc

all: contactbook.o libcontactbook.a libcontactbook.so clean

libcontactbook.a: contactbook.o
	ar rcs $@ $^

libcontactbook.so: contactbook.c
	gcc -fPIC -shared $^ -o $@

contactbook.o: contactbook.c
	gcc -o $@ -c $^

contacttest: 
	rm -f *.so
	rm -f contacttest
	gcc main.c -L. -lcontactbook -Wall -ansi -std=c99 -o $@
	./contacttest

testprogram: clean all contacttest

coredump:
	ulimit -c unlimited
	gcc main.c -L. -lcontactbook -g -Wall -ansi -std=c99 -o contacttest
	./contacttest

rundebug:
	gdb -c core ./contacttest

.PHONY: clean
clean:
	rm -f *.o
	rm -f contacttest
	rm -f core
