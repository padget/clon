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
LIBS        := 
FLAGS       := -std=c++20 -Wall -pedantic -Werror -O3 -fconcepts-diagnostics-depth=4
VERSION     := $(shell more clon.hpp | grep CLON_VERSION | grep -Po '[0-9]+\.[0-9]+\.[0-9]+')
DIST_PREFIX := libclon
DIST		:= $(DIST_PREFIX)-$(VERSION).zip

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
	rm -f *.o *.out

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
# 		TEST TARGETS
###########################################

format.test.out: ##@test build format.test.out that package all unit-tests for format.
format.test.out: format.test.cpp format.hpp test.hpp
	${CC} -o $@ $< ${LIBS} ${FLAGS}

.PHONY: format.test
format.test: format.test.out
	./$^

clon.test.out: ##@test build clon.test.o that package all unit-tests for clon.
clon.test.out: clon.test.cpp clon.hpp test.hpp
	${CC} -o $@ $< ${LIBS} ${FLAGS}

.PHONY: clon.test
clon.test: clon.test.out 
	./$^

.PHONY: test
test: ##@test run all unit-tests.
test: format.test clon.test

###########################################
# 		DIST TARGETS
###########################################

.PHONY: dist
dist: ##@dist compress/zip format.hpp clon.hpp utils.hpp README.md LICENSE into $(DIST).
dist: format.hpp clon.hpp utils.hpp README.md LICENSE
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
	@$(MAKE) test
	@$(MAKE) dist 
	@$(MAKE) bench
	@$(MAKE) install
	@$(MAKE) clean-temporaries


