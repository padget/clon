#@###########################################
#@#                                         #
#@#                                         #
#@#    ####### ###     ####### ####  ###    #
#@#    ####### ###     ####### ####  ###    #
#@#    ###     ###     ### ### ##### ###    #
#@#    ###     ###     ### ### #########    #
#@#    ###     ###     ### ### ### #####    #
#@#    ####### ####### ####### ###  ####    #
#@#    ####### ####### ####### ###  ####    #
#@#                                         #
#@#                                         #
#@###########################################

#@###########################################
#@# Makefile of the libclon projet.         #
#@# Using  : make all                       #
#@# Author : Benjamin MATHIEU               #
#@# Email  : padget.pro@gmail.com           #
#@###########################################

###########################################
# VARIABLES
###########################################

CC          := g++-10
LIBS        := -lfmt
FLAGS       := -std=c++20 -Wall -pedantic -Werror -O3
VERSION     := $(shell more clon.hpp | grep CLON_VERSION | grep -Po '[0-9]+\.[0-9]+\.[0-9]+')
DIST_PREFIX := libclon
DIST				:= $(DIST_PREFIX)-$(VERSION).zip

# $@ contient le target
# $^ contient les dépendances
# $< contient la première dépendance

###########################################
# TARGETS
###########################################

###########################################
# 		CLEAN TARGETS
###########################################

# Add the following 'help' target to your Makefile
# And add help text after each target name starting with '\#\#'
# A category can be added with @category
# Credits to: https://gist.github.com/prwhite/8168133

PRINT_META = \
	while(<>) { \
		if (/^\#@(.*)$$/) { \
			print "$$1\n"; \
		} \
	}; \
	print "\n";\
	
HELP_FUNC = \
	%help; \
	while(<>) { \
		if(/^([a-z0-9._-]+):.*\#\#(?:@(\w+))?\s(.*)$$/) { \
			push(@{$$help{$$2}}, [$$1, $$3]); \
		} \
	}; \
	print "Usage: make ${YELLOW}[target]${RESET}\n\n"; \
	for ( sort keys %help ) { \
		print "${WHITE}$$_:${RESET}\n"; \
		printf("  ${YELLOW}%-20s${RESET} ${GREEN}%s${RESET}\n", $$_->[0], $$_->[1]) for @{$$help{$$_}}; \
		print "\n"; \
	}

.PHONY: help
help: ##@options Shows a list of all available make options.
	@perl -e '$(PRINT_META)' $(MAKEFILE_LIST)
	@perl -e '$(HELP_FUNC)' $(MAKEFILE_LIST)
###########################################
# 		CLEAN TARGETS
###########################################

.PHONY: clean-temporaries
clean-temporaries: ##@clean clean the temporary generated filess.
clean-temporaries:
	rm -f *.o *.test.out *.bench.out

.PHONY: clean-doc
clean-doc: ##@clean clean the generated documentation.
clean-doc:

.PHONY: clean-dist
clean-dist: ##@clean clean the $(DIST) archive.
clean-dist:
	rm -f $(DIST_PREFIX)-*.zip

.PHONY: clean	
clean: ##@clean clean the all project generated files (temp, doc, dist, .out).
clean: clean-temporaries clean-doc clean-dist

###########################################
# 		BUILD TARGETS
###########################################

clon.o: ##@build build the clon.o library.
clon.o: clon.cpp clon.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

model.o: ##@build build the model.o library.
model.o: model.cpp model.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

parsing.o: ##@build build the parsing.o library.
parsing.o: parsing.cpp parsing.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

path.o: ##@build build the path.o library.
path.o: path.cpp path.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

inout.o: ##@build build the intout.o library.
inout.o: inout.cpp inout.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

utils.o: ##@build build the utils.o library.
utils.o: utils.cpp utils.hpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

libclon.o: ##@build build the libclon.o library.
libclon.o: clon.o model.o parsing.o utils.o path.o inout.o  
	ld -relocatable $^ -o $@

.PHONY: build
build: ##@build build the libclon.o library.
build: libclon.o

###########################################
# 		TEST TARGETS
###########################################


clon.test.o: ##@test build clon.test.o that package all unit-tests.
clon.test.o: clon.test.cpp
	${CC} -o $@  -c $< ${LIBS} ${FLAGS}

clon.test.out: ##@test build clon.test.out to execute all unit-tests.
clon.test.out: clon.test.o libclon.o
	@${CC} -o $@ $^ ${LIBS}

.PHONY: test
test: ##@test test ibclon.o with the all unit tests.
test: clon.test.out
	./$<

###########################################
# 		DIST TARGETS
###########################################

.PHONY: dist
dist: ##@dist compress/zip libclon.o, README.md and LICENSE into $(DIST).
dist: libclon.o README.md LICENSE
	zip $(DIST) $^

###########################################
# 		INSTALL TARGETS
###########################################

.PHONY: install
install: ##@ 
install:

###########################################
# 		BENCH TARGETS
###########################################

.PHONY: bench
bench: ##@
bench: 

###########################################
#     ALL TARGETS
###########################################

.PHONY: version
version: ##@version display the version of libclon.o.
version: clon.hpp
	@echo version of application : ${VERSION}

.PHONY: all
all: ##@all execute all targets in the right order : clean > build > test > dist > bench > install > clean-temporaries.

all: version	
	@$(MAKE) clean
	@$(MAKE) build
	@$(MAKE) test
	@$(MAKE) dist 
	@$(MAKE) bench
	@$(MAKE) install
	@$(MAKE) clean-temporaries


## FORMAT target. TODO merge with other targets
format.out: format.test.cpp format.hpp
	${CC} -o $@  $< ${LIBS} ${FLAGS}
	./$@
	@rm -f format.out

format: format.out

