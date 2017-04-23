# Nathaniel Want (nwqk6)
# CS4280-E02
# P4
# 4/24/2017
CC=g++
FLAGS=-std=c++11
EXE=testSem #TODO: rename exe
OBJS=scannerclass.o scanner.o parser.o sem.o ST.o token.o codegen.o #treePrint.o
MAIN=main.cpp errors.cpp
TMP=
M1=scanner
M1FILES=scannerclass.cpp scanner.cpp #treePrint.cpp
M2=parser
M2FILES=parser.cpp
M3=backEnd
M3FILES=sem.cpp codegen.cpp 
M4=token
M4FILES=token.cpp
M5=ST
M5FILES=ST.cpp
#M6=codegen
#M6FILES=codegen.cpp



$(EXE) : $(MAIN) $(M1) $(M2) $(M3) $(M4) $(M5) #$(M6)
	$(CC) $(FLAGS) -g $(MAIN) $(OBJS) -o $(EXE)

$(M1): $(M1FILES) 
	$(CC) $(FLAGS) -g -c $(M1FILES)

$(M2): $(M2FILES)
	$(CC) $(FLAGS) -g -c $(M2FILES)

$(M3): $(M3FILES)
	$(CC) $(FLAGS) -g -c $(M3FILES)

$(M4): $(M4FILES)
	$(CC) $(FLAGS) -g -c $(M4FILES)

$(M5): $(M5FILES)
	$(CC) $(FLAGS) -g -c $(M5FILES)

#$(M6): $(M6FILES)
#	$(CC) $(FLAGS) -g -c $(M6FILES)

clean:
	rm -rf $(EXE) *.o $(TMP)
