# Nathaniel Want (nwqk6)
# CS4280-E02
# P4
# 4/24/2017
CC=g++
FLAGS=-std=c++11
EXE=testSem #TODO: rename exe
OBJS=scannerclass.o scanner.o parser.o ST.o codegen.o 
MAIN=main.cpp errors.cpp token.cpp 
TMP=
M1=frontEnd
M1FILES=scannerclass.cpp scanner.cpp parser.cpp
M2=backEnd
M2FILES=codegen.cpp ST.cpp


$(EXE) : $(MAIN) $(M1) $(M2) 
	$(CC) $(FLAGS) -g $(MAIN) $(OBJS) -o $(EXE)

$(M1): $(M1FILES) 
	$(CC) $(FLAGS) -g -c $(M1FILES)

$(M2): $(M2FILES)
	$(CC) $(FLAGS) -g -c $(M2FILES)

clean:
	rm -rf $(EXE) *.o $(TMP)
