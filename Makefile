# Copyright (c) 2015, Monaco F. J. <moanco@icmc.usp.br>
# Modified by Matheus G. S. Beleboni

# Default target

all: main

# Objects

OBJECTS = main.o parser.o bottom-left.o

# Binary

main : $(OBJECTS)
	g++ $^ -o $@

# Pattern rule for object-source dependences

%.o : %.cpp 
	g++ -I. -c $<

# Automatic object-header dependences

makefiles = $(OBJECTS:%.o=%.d) 

include $(makefiles)

%.d : %.cpp
	g++ -I. -MM -MT '$(<:.cpp=.o) $@' $< >$@

# Cleaning

clean:
	rm -f *.o main
	rm -f *~ \#*
	rm -f *.d
	rm -f solution.*
	rm -f *.tex
	rm -f adding_*.*
	rm -f NFP_*.*
