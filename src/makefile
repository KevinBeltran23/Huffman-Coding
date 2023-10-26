CC = gcc

CFlags = -Wall -ansi -pedantic -g

LD = gcc

LDFLAGS = 

.SILENT:

all: hencode hdecode

hdecode: linkedList.o utility.o decoder.o
	$(LD) $(LDFLAGS) -o hdecode linkedList.o utility.o decoder.o

hencode: linkedList.o utility.o encoder.o
	$(LD) $(LDFLAGS) -o hencode linkedList.o utility.o encoder.o

linkedList.o: linkedList.c
	$(CC) -c -o $(CFLAGS) linkedList.o linkedList.c

utility.o: utility.c
	$(CC) -c -o $(CFLAGS) utility.o utility.c

decoder.o: decoder.c
	$(CC) -c -o $(CFLAGS) decoder.o decoder.c

encoder.o: encoder.c
	$(CC) -c -o $(CFLAGS) encoder.o encoder.c

testdecode: hdecode
	valgrind --leak-check=full --track-origins=yes ./hdecode henout hdeout

testencode: hencode
	valgrind --leak-check=full --track-origins=yes ./hencode TestBinary henout

clean: 
	rm -f *.o htable
