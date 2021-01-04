test: main.o mock.o lcd.o
	arm-linux-gcc lcd.o main.o mock.o -o test
main.o: main.c
	arm-linux-gcc main.c -o main.o -c -g
mock.o: mock.c
	arm-linux-gcc mock.c -o mock.o -c -g
lcd.o: lcd.c
	arm-linux-gcc lcd.c -o lcd.o -c -g
all: test

clean:
	rm -fr *.o
	rm -fr test