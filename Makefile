SHELL=/bin/sh
CXX = g++
C = gcc

exec        : main.o Prog.o MixHy.o IterAlph.o Mesh.o ConjGrad.o Visu.o mylibrary.o  y.tab.o lex.yy.o 
	$(CXX) -g -o 2dres main.o Prog.o MixHy.o IterAlph.o Mesh.o ConjGrad.o Visu.o mylibrary.o  y.tab.o lex.yy.o -lm

main.o      : main.cc
	$(CXX) -g -c -o main.o main.cc 

Prog.o      : Prog.cc Prog.hh MixHy.hh IterAlph.hh Mesh.hh Visu.hh
	$(CXX) -g -c -o Prog.o Prog.cc 

MixHy.o     : MixHy.cc MixHy.hh
	$(CXX) -g -c -o MixHy.o MixHy.cc 

IterAlph.o  : IterAlph.cc IterAlph.hh
	$(CXX) -g -c -o IterAlph.o IterAlph.cc 

ConjGrad.o  : ConjGrad.cc ConjGrad.hh
	$(CXX) -g -c -o ConjGrad.o ConjGrad.cc 

Mesh.o      : Mesh.cc Mesh.hh
	$(CXX) -g -c -o Mesh.o Mesh.cc 

Visu.o      : Visu.cc Visu.hh
	$(CXX) -g -c -o Visu.o Visu.cc

mylibrary.o : mylibrary.hh
	$(CXX) -g -c -o mylibrary.o mylibrary.cc 

# below is needed to compile the config file parsing module
# see ./cfp/cfp.h for more info

y.tab.o: y.tab.c  cfp/cfp.h
	${C} ${CFLAGS} -c -g y.tab.c 

lex.yy.o: lex.yy.c cfp/cfp.h  
	${C} ${CFLAGS} -c -g lex.yy.c

lex.yy.c: cfp/cfp.l
	flex cfp/cfp.l

y.tab.c: cfp/cfp.y
	yacc -d cfp/cfp.y

.PHONY: clean

clean:
	rm -f 2dres *.o *~ lex.yy.* y.tab.*
