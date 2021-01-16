CC          := g++-10
LIBS        := -lfmt
FLAGS       := -std=c++20 -Wall -pedantic -Werror
VERSION     := $(shell more clon.hpp | grep CLON_VERSION | grep -Po '[0-9]+\.[0-9]+\.[0-9]+')
DISTARCHIVE := clon-$(VERSION).zip

# $@ contient le target
# $^ contient les dépendances
# $< contient la première dépendance

.PHONY: clean-temporaries
clean-temporaries:
	rm -f *.o *.test.out *.bench.out

.PHONY: clean-doc
clean-doc:

.PHONY: clean-dist
clean-dist:
	rm -f $(DISTARCHIVE)

.PHONY: clean	
clean: clean-temporaries clean-doc clean-dist

clon.o: clon.cpp clon.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

model.o: model.cpp model.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

parsing.o: parsing.cpp parsing.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

path.o: path.cpp path.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

inout.o: inout.cpp inout.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

utils.o: utils.cpp utils.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

libclon.o: clon.o model.o parsing.o utils.o #inout.o  path.o
	ld -relocatable $^ -o $@

.PHONY: build
build: libclon.o

clon.test.o: clon.test.cpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

clon.test.out: clon.test.o libclon.o
	@${CC} -o $@ $^ ${LIBS}

.PHONY: test
test: clon.test.out
	./$<

.PHONY: version
version: clon.hpp
	@echo version of application : ${VERSION}

.PHONY: dist
dist: libclon.o
	zip clon-$(VERSION).zip $^

.PHONY: all
all: version	
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) test
	$(MAKE) dist 
	$(MAKE) clean-temporaries
