GCC := gcc
DEBUG := -Wall -O0 -g
SRC_FOLDER:= ./src/DomainBoard/
BUILD_FOLDER:= ./build/

all: buildDB

buildDB: $(BUILD_FOLDER)test
	cp $(SRC_FOLDER)/config.txt $(BUILD_FOLDER)
$(BUILD_FOLDER)test: $(BUILD_FOLDER)main.o $(BUILD_FOLDER)mock.o
	$(GCC) $(DEBUG) $(SRC_FOLDER)main.o $(SRC_FOLDER)mock.o -o $(BUILD_FOLDER)test 
$(BUILD_FOLDER)main.o: $(SRC_FOLDER)main.c
	$(GCC) $(DEBUG) $(SRC_FOLDER)main.c -o $(BUILD_FOLDER)main.o -c
$(BUILD_FOLDER)mock.o: $(SRC_FOLDER)mock.c
	$(GCC) $(DEBUG) $(SRC_FOLDER)mock.c -o $(BUILD_FOLDER)mock.o -c
$(BUILD_FOLDER)lcd.o: $(SRC_FOLDER)lcd.c
	$(GCC) $(DEBUG) $(SRC_FOLDER)lcd.c -o $(BUILD_FOLDER)lcd.o -c

run: buildDB
	cd ./build && ./test

clean:
	rm -fr build