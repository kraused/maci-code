#!/bin/bash

for v in `ls envs`;
do
    ${MACI}/tests/testing.py ./envs/$v &
	# Sleep for hundred seconds to make sure that the makedir()
	# calls do not overlap which can make problems
	sleep 100
done

