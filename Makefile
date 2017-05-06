# Nathaniel Want (nwqk6)
# CS4280-E02
# P4
# 5/9/2017
CC=g++
FLAGS=-std=c++11
EXE=comp 
OBJS=scannerclass.o scanner.o parser.o SysStack.o ST.cpp codegen.o 
MAIN=main.cpp errors.cpp token.cpp 
TMP=*.asm test/*.asm
FE=frontEnd
FEFILES=scannerclass.cpp scanner.cpp parser.cpp
BE=backEnd
BEFILES=codegen.cpp SysStack.cpp ST.cpp


$(EXE) : $(MAIN) $(FE) $(BE) 
	$(CC) $(FLAGS) -g $(MAIN) $(OBJS) -o $(EXE)

$(FE): $(FEFILES) 
	$(CC) $(FLAGS) -g -c $(FEFILES)

$(BE): $(BEFILES)
	$(CC) $(FLAGS) -g -c $(BEFILES)

clean:
	rm -rf $(EXE) *.o $(TMP)
