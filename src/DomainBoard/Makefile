test: main.o mock.o
	gcc main.o mock.o -o test
main.o: main.c
	gcc main.c -o main.o -c
mock.c: mock.c
	gcc mock.c -o mock.o -c
all: test

clean:
	rm -fr *.o