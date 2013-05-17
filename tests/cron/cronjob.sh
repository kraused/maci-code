#!/bin/bash

cd <MACI TEST FOLDER>

# The current time
NOW=`date +"%a-%b-%y-%H-%M"`

# We need the newer subversion client
module load svn/1.6.9
./update_all.sh &> update_all_$NOW.log

./build_all.sh  &> build_all_$NOW.log
./test_all.sh   &> test_all_$NOW.log

wait

