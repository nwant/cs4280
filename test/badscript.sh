#!/bin/bash

for i in {1..9}
do
	./comp test/bad$i
done

#test static semantics files (P3)
./comp test/testSem_bad_1
./comp test/testSem_bad_2
./comp test/testSem_bad_3

