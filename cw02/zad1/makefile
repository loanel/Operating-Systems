CC = gcc

all: systemver.o libraryver.o libsystemver.a liblibraryver.a libraries main
libraries:
	ar rcs liblibraries.a systemver.o libraryver.o
liblibraryver.a: libraryver.o
	ar rcs $@ $^

libsystemver.a: systemver.o
	ar rcs $@ $^

libraryver.o: libraryver.c
	gcc -o $@ -c $^

systemver.o: systemver.c
	gcc -o $@ -c $^

main:
	gcc -Wall main.c -L. -llibraries -o main
test:
	./main sys generate datafile 1000 4
	cp datafile temporary
	./main sys sort datafile 1000 4
	rm datafile
	mv temporary datafile
	./main lib sort datafile 1000 4
	./main sys shuffle datafile 1000 4
	./main lib shuffle datafile 1000 4
	rm datafile

	./main sys generate datafile 5000 4
	cp datafile temporary
	./main sys sort datafile 5000 4
	rm datafile
	mv temporary datafile
	./main lib sort datafile 5000 4
	./main sys shuffle datafile 5000 4
	./main lib shuffle datafile 5000 4
	rm datafile

	./main sys generate datafile 1000 512
	cp datafile temporary
	./main sys sort datafile 1000 512
	rm datafile
	mv temporary datafile
	./main lib sort datafile 1000 512
	./main sys shuffle datafile 1000 512
	./main lib shuffle datafile 1000 512
	rm datafile

	./main sys generate datafile 5000 512
	cp datafile temporary
	./main sys sort datafile 5000 512
	rm datafile
	mv temporary datafile
	./main lib sort datafile 5000 512
	./main sys shuffle datafile 5000 512
	./main lib shuffle datafile 5000 512
	rm datafile

	./main sys generate datafile 1000 4096
	cp datafile temporary
	./main sys sort datafile 1000 4096
	rm datafile
	mv temporary datafile
	./main lib sort datafile 1000 4096
	./main sys shuffle datafile 1000 4096
	./main lib shuffle datafile 1000 4096
	rm datafile

	./main sys generate datafile 5000 4096
	cp datafile temporary
	./main sys sort datafile 5000 4096
	rm datafile
	mv temporary datafile
	./main lib sort datafile 5000 4096
	./main sys shuffle datafile 5000 4096
	./main lib shuffle datafile 5000 4096
	rm datafile

	./main sys generate datafile 1000 8192
	cp datafile temporary
	./main sys sort datafile 1000 8192
	rm datafile
	mv temporary datafile
	./main lib sort datafile 1000 8192
	./main sys shuffle datafile 1000 8192
	./main lib shuffle datafile 1000 8192
	rm datafile

	./main sys generate datafile 5000 8192
	cp datafile temporary
	./main sys sort datafile 5000 8192
	rm datafile
	mv temporary datafile
	./main lib sort datafile 5000 8192
	./main sys shuffle datafile 5000 8192
	./main lib shuffle datafile 5000 8192
	rm datafile



.PHONY: clean
clean:
	rm -f *.o
	rm -f *.a
	rm -f main
	rm -f datafile
