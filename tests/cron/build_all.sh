#!/bin/bash

# Assume that we are in the maci_testing
# directory

test -d build && rm -rf build
mkdir   build

cd      build

for v in `ls ../config`
do
	${MACI}/config/build.py ../config/$v -j 2
done

