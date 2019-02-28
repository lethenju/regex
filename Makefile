# Makefile for Regex source and test
# MIT License
# Julien LE THENO

all: clean lib test
# Chosen example

SHELL:=/bin/bash
BUILD_DIR=build
OBJECTS_DIR=$(BUILD_DIR)/obj
EXE_DIR=$(BUILD_DIR)/exe
SRC_DIR=src
TEST_DIR=test
F1_EXISTS=$(shell [ -e $(BUILD_DIR) ] && echo Y || echo N )

.PHONY: all lib clean

# TEST
test: lib test.o
	gcc -o $(EXE_DIR)/test_exe $(OBJECTS_DIR)/*

test.o: $(TEST_DIR)/test.c $(SRC_DIR)/*.h 
	gcc -g -c $(TEST_DIR)/test.c -o  $(OBJECTS_DIR)/test.o


### LIB TARGET
lib: setup regex.o

regex.o: $(SRC_DIR)/regex.c $(SRC_DIR)/regex.h
	gcc -g -c $(SRC_DIR)/regex.c -o  $(OBJECTS_DIR)/regex.o

### END LIB TARGET


setup:
ifeq ($(F1_EXISTS),N)
	@echo "Creating build directory"
	mkdir $(BUILD_DIR)
	mkdir $(EXE_DIR)
	mkdir $(OBJECTS_DIR)
endif

make clean:
	rm -f $(EXE_DIR)/*
	rm -f $(OBJECTS_DIR)/*

make rebuild: clean all
