SHELL := /bin/bash
PWD := $(shell pwd)
CC = gcc
CXX = g++
UNAME := $(shell uname -s)
CXXBLDFLAGS = -Wall -Wextra -std=c++14
CXXBLDDFLAGS = -Wall -Wextra -std=c++14 -pedantic
CXXFLAGS = -D__STDC_CONSTANT_MACROS -D__STDINT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -O3
CXXDFLAGS = -D__STDC_CONSTANT_MACROS -D__STDINT_MACROS -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1 -O3 -DDEBUG_FLAG=1
INCLUDES = /usr/include

ifeq ($(UNAME),Darwin)
	CC = clang
	CXX = clang++
	FLAGS += -Weverything
else
	CXXFLAGS += -static
endif

all: rts

rts:
	$(CXX) -g $(CXXBLDFLAGS) $(CXXFLAGS) -c rts.cpp -o rts.o
	$(CXX) -g $(CXXBLDFLAGS) $(CXXFLAGS) -I$(INCLUDES) rts.o -o rts

test-lower:
	set -e; \
	ROWS=$$(wc -l ./test.mtx | awk '{print ($$1-1)}'); \
	COLS=$$(tail -1 ./test.mtx | awk '{print NF-1}'); \
	./rts --rows $${ROWS} --cols $${COLS} --samples 20 --order 3 --rng-seed 123 --lower --preserve-metadata < ./test.mtx

test-upper:
	set -e; \
	ROWS=$$(wc -l ./test.mtx | awk '{print ($$1-1)}'); \
	COLS=$$(tail -1 ./test.mtx | awk '{print NF-1}'); \
	./rts --rows $${ROWS} --cols $${COLS} --samples 50 --order 4 --rng-seed 123 --upper --preserve-metadata < ./test.mtx

clean:
	rm -rf *~
	rm -rf rts
	rm -rf rts.o