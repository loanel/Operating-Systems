CC = gcc

all: contactbook.o libcontactbook.a libcontactbook.so clean

libcontactbook.a: contactbook.o
	ar rcs $@ $^

libcontactbook.so: contactbook.c
	gcc -fPIC -shared $^ -o $@

contactbook.o: contactbook.c
	gcc -o $@ -c $^

contacttestStatic: 
	gcc main.c -static -L. -lcontactbook -Wall -ansi -std=c99 -o $@
	./contacttest > staticmeasurement.txt

contacttestShared:
	gcc main.c -L. -Wl,-rpath=. -lcontactbook -o contacttestShared
	./contacttestShared > sharedmeasurement.txt

contacttestDL:
	gcc main2.c -ldl -o DLcontacttest
	./DLcontacttest > DLmeasurement.txt

testprogram: clean all contacttest4

contacttestStaticOzero:
	gcc main.c -static -L. -lcontactbook -Wall -ansi -O0 -std=c99 -o $@

contacttestStaticOtwo:
	gcc main.c -static -L. -lcontactbook -Wall -ansi -O2 -std=c99 -o $@

contacttestStaticOthree:
	gcc main.c -static -L. -lcontactbook -Wall -ansi -O3 -std=c99 -o $@

contacttestStaticOfast:
	gcc main.c -static -L. -lcontactbook -Wall -ansi -Ofast -std=c99 -o $@

contacttestSharedOzero:
	gcc main.c -L. -Wl,-rpath=. -lcontactbook -O0 -o $@
	
contacttestSharedOtwo:
	gcc main.c -L. -Wl,-rpath=. -lcontactbook -O2 -o $@

contacttestSharedOthree:
	gcc main.c -L. -Wl,-rpath=. -lcontactbook -O3 -o $@

contacttestSharedOfast:
	gcc main.c -L. -Wl,-rpath=. -lcontactbook -Ofast -o $@

contacttestDLOzero:
	gcc main2.c -ldl -O0 -o $@

contacttestDLOtwo:
	gcc main2.c -ldl -O2 -o $@

contacttestDLOthree:
	gcc main2.c -ldl -O3 -o $@

contacttestDLOfast:
	gcc main2.c -ldl -Ofast -o $@

tests: contacttestStaticOzero contacttestStaticOtwo contacttestStaticOthree contacttestStaticOfast contacttestSharedOzero contacttestSharedOtwo contacttestSharedOthree contacttestSharedOfast contacttestDLOzero contacttestDLOtwo contacttestDLOthree contacttestDLOfast

staticTests: 
	./contacttestStaticOzero >> staticTests.txt
	./contacttestStaticOtwo >> staticTests.txt
	./contacttestStaticOthree >> staticTests.txt
	./contacttestStaticOfast >> staticTests.txt

sharedTests: 
	./contacttestSharedOzero >> sharedTests.txt
	./contacttestSharedOtwo >> sharedTests.txt
	./contacttestSharedOthree >> sharedTests.txt
	./contacttestSharedOfast >> sharedTests.txt

dlTests:
	./contacttestDLOzero >> dlTests.txt
	./contacttestDLOtwo >> dlTests.txt
	./contacttestDLOthree >> dlTests.txt
	./contacttestDLOfast >> dlTests.txt


.PHONY: clean
clean:
	rm -f *.o
	rm -f contacttest
	rm -f contacttest*
