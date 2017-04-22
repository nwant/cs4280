#!/bin/bash

for i in {1..9}
do
	./testSem test/bad$i
done
