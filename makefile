CC          := g++-10 
LIBS        := 
FLAGS       := -std=c++20 -Wall -pedantic -Werror -O3
VERSION     := $(shell more clon.hpp | grep CLON_VERSION | grep -Po '[0-9]+\.[0-9]+\.[0-9]+')
DIST_PREFIX := libclon
DIST		:= $(DIST_PREFIX)-$(VERSION).zip

# $@ contient le target
# $^ contient les dépendances
# $< contient la première dépendance

.PHONY: clean	clean-dist clean-temporaries

clean-temporaries:
	rm -f *.o *.out

clean-dist:
	rm -f $(DIST_PREFIX)-*.zip

clean: clean-temporaries clean-dist

format.test.out: format.test.cpp format.hpp test.hpp
	${CC} -o $@ $< ${LIBS} ${FLAGS}

.PHONY: format.test

format.test: format.test.out
	./$^

clon.test.out: clon.test.cpp clon.hpp test.hpp
	${CC} -o $@ $< ${LIBS} ${FLAGS}

.PHONY: clon.test

clon.test: clon.test.out 
	./$^

.PHONY: test

test: format.test clon.test

.PHONY: dist

dist: format.hpp clon.hpp utils.hpp README.md LICENSE
	zip $(DIST) $^

.PHONY: version

version: clon.hpp
	@echo version of application : ${VERSION}

.PHONY: all

all: version
	@$(MAKE) clean
	@$(MAKE) test
	@$(MAKE) dist 
	@$(MAKE) clean-temporaries


