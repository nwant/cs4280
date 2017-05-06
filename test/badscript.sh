#!/bin/bash

for i in {1..9}
do
	./comp test/bad$i
done

#test static semantics files (P3)
./comp test/testSem_bad1
./comp test/testSem_bad2
./comp test/testSem_bad3

