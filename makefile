#TODO Reorganiser les targets dans un ordre logique
COMPILER=g++-10
LIBS=-lfmt
FLAGS=-std=c++20 -Wall -O3
VERSION=`more clon.hpp | grep CLON_VERSION | grep -Po '[0-9]+\.[0-9]+\.[0-9]+'`
TITLESEP="------------------------------------------"

.PHONY: all build test clean start version archive

all: start clean build test version archive

version: clon.hpp
	@echo version of application : ${VERSION}

archive: zip-check clon.hpp clon.cpp dist/clon.o version
	@zip dist/clon-${VERSION}.zip clon.hpp clon.cpp dist/clon.o 
	@echo archive created clon-${VERSION}.zip

zip-check-title:
	@echo ${TITLESEP}
	@echo "-- check zip existence"
	@echo ${TITLESEP}

zip-check: zip-check-title
	@if [ -z `which zip` ]; then\
		sudo apt install zip;\
	fi
	@echo ' -> Use of' `which zip`

compiler-check-title:
	@echo ${TITLESEP}
	@echo "-- check ${COMPILER} existence"
	@echo ${TITLESEP}

compiler-check: compiler-check-title
	@if [ -z `which ${COMPILER}` ]; then\
		sudo apt install ${COMPILER};\
	fi
	@echo ' -> Use of' `which ${COMPILER}`

start: 
	@echo ${TITLESEP}
	@echo "- clon library"
	@echo ${TITLESEP}

build_title: 
	@echo ${TITLESEP}
	@echo "-- build clon library"
	@echo ${TITLESEP}
	
build: compiler-check build_title dist/clon.o
	
test_title:
	@echo ${TITLESEP}
	@echo "-- unit tests"
	@echo ${TITLESEP}
	
test: compiler-check test_title tests/test.clon.out
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
	@echo " -> remove archives *.zip"
	@rm -f *.zip
