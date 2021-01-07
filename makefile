COMPILER=g++-10
LIBS=-lfmt
FLAGS=-std=c++20 -Wall -O3
TITLESEP="------------------------------------------"
.PHONY: all build test clean start

all: start clean build test

start: 
	@echo ${TITLESEP}
	@echo "- clon library"
	@echo ${TITLESEP}

build_title: 
	@echo ${TITLESEP}
	@echo "-- build clon library"
	@echo ${TITLESEP}
	
build: build_title dist/clon.o
	
test_title:
	@echo ${TITLESEP}
	@echo "-- unit tests"
	@echo ${TITLESEP}
	
test: test_title tests/test.clon.out
	@echo " -> running clon tests..."
	@./tests/test.clon.out

dist:
	@echo " -> creation of dist directory..."
	@mkdir dist

tests:
	@echo " -> creation of tests directory..."
	@mkdir tests

dist/clon.o: dist clon.cpp clon.hpp
	@echo " -> building dist/clon.o..."
	@${COMPILER} -o dist/clon.o  -c clon.cpp ${LIBS} ${FLAGS}

tests/test.clon.o: tests test.clon.cpp
	@echo " -> building tests/test.clon.o..." 
	@${COMPILER} -o tests/test.clon.o -c test.clon.cpp ${FLAGS}

tests/test.clon.out: tests tests/test.clon.o dist/clon.o
	@echo " -> building tests/test.clon.out..."
	@${COMPILER} -o tests/test.clon.out tests/test.clon.o dist/clon.o  ${LIBS} ${FLAGS} 

clean_title:
	@echo ${TITLESEP}
	@echo "-- clean project"
	@echo ${TITLESEP}
	

clean: clean_title
	@echo " -> remove dist directory..."
	@rm -rf dist
	@echo " -> remove tests directory..."
	@rm -rf tests

