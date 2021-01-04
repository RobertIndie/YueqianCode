test: main.o mock.o
	gcc main.o mock.o -o test
main.o: main.c
	gcc main.c -o main.o -c -g
mock.o: mock.c
	gcc mock.c -o mock.o -c -g
all: test

clean:
	rm -fr *.o
	rm -fr test