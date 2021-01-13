GCC := gcc
DEBUG := -Wall -O0 -g

test: main.o mock.o
	$(GCC) $(DEBUG) main.o mock.o -o test 
main.o: main.c
	$(GCC) $(DEBUG) main.c -o main.o -c
mock.o: mock.c
	$(GCC) $(DEBUG) mock.c -o mock.o -c
lcd.o: lcd.c
	$(GCC) $(DEBUG) lcd.c -o lcd.o -c
all: test

clean:
	rm -fr *.o
	rm -fr test