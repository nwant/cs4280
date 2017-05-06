#!/bin/bash

# perform all test files created by me (P1 - P2)
for i in {1..36}
do
	./comp test/good$i
done

#test static semantics files (P3)
./comp test/testSem_good1
./comp test/testSem_good2

#test all code gen files (P4)
./comp test/111
./comp test/assign1
./comp test/blockEcho
./comp test/echoIfPos
./comp test/echoIfTeen
./comp test/inputDownToZeroRec
./comp test/inputsReversed
./comp test/inputToZero
./comp test/negEcho
./comp test/print1
